#include <fmt/format.h>
#include <parser/loop_branch.h>
#include <parser/parser.h>
#include <runtime/mem.h>
namespace cake {

void ControlFlowNode::flatten_blocks(std::vector<AstNodePtr> &stmts) {
  std::vector<AstNodePtr> src;
  std::swap(stmts, src);

  for (auto &node : src) {
    if (auto cfg = dynamic_cast<ControlFlowNode *>(node.get()))
      cfg->generate_to(stmts);
    else
      stmts.emplace_back(std::move(node));
  }
}

AstNodePtr Parser::parse_if() {
  AstNodePtr cur_cond;
  vector<AstNodePtr> cur_blk;

  auto ret = std::make_unique<IfStmt>();
  match(IF);
  match(LPAR);
  cur_cond = parse_expr();
  match(RPAR);
  ret->conditional_blocks.push_back({std::move(cur_cond), parse_block()});

  while (peek(0).kind == TokenKind::ELSE) {
    if (peek(1).kind == TokenKind::IF) {
      match(ELSE);
      match(IF);
      match(LPAR);
      auto cur_cond = parse_expr();
      match(RPAR);
      ret->conditional_blocks.emplace_back(std::move(cur_cond), parse_block());
    } else {
      match(ELSE);
      auto blk = parse_block();
      ret->else_block = std::move(blk);
      break;
    }
  }
  return ret;
}

AstNodePtr Parser::parse_while() {
  auto ret = std::make_unique<WhileStmt>();
  match(WHILE);
  match(LPAR);
  ret->condition = parse_expr();
  match(RPAR);
  ret->loop_body = parse_block();
  return ret;
}

TmpObjectPtr Goto::eval() {
  Memory::pc = dest;
  return nullptr;
}

std::string IfWithJmpTable::to_string() const {
  std::string ret = "(jmp-tab ";
  for (auto &[node, dest] : jmp_tab) {
    ret += fmt::format("({} {}) ", node->to_string(), dest);
  }
  return ret + ")";
}
TmpObjectPtr IfWithJmpTable::eval() {
  for (auto &[node, dest] : jmp_tab) {
    auto val = node->eval();
    bool res = val->is_true();

    if (res) {
      Memory::pc = dest;
      return nullptr;
    }
  }
  Memory::pc = false_dest;
  return nullptr;
}

void IfStmt::generate_to(vector<AstNodePtr> &block) {
  /*
    block ...
    ifwith jmp table
        cond 1 goto tag1
        cond 2 goto tag2
    goto end/else_tag;
  tag 1:
    goto end;
  tag 2:
    goto end;
  tag 3:
    goto end;
  else_tag
  termilate:
  */
  AstNode *conditional_jmp;
  int *else_jmp;

  if (conditional_blocks.size() == 1) {
    conditional_jmp = new IfTrueJmp();
    else_jmp = &static_cast<IfTrueJmp *>(conditional_jmp)->false_dest;
  } else {
    conditional_jmp = new IfWithJmpTable();
    else_jmp = &static_cast<IfWithJmpTable *>(conditional_jmp)->false_dest;
  }

  block.push_back(AstNodePtr(conditional_jmp));

  vector<int> pos_vec;
  vector<int *> need_to_point_to_end;
  for (auto &[cond, blk] : conditional_blocks) {
    pos_vec.push_back(block.size() - 1);
    for (auto &node : blk) {
      auto cfg = dynamic_cast<ControlFlowNode *>(node.get());
      if (cfg)
        cfg->generate_to(block);
      else
        block.push_back(std::move(node));
    }
    auto go_end = std::make_unique<Goto>();
    need_to_point_to_end.push_back(&go_end->dest);
    block.push_back(std::move(go_end));
  }

  *else_jmp = block.size() - 1;
  for (auto &node : else_block) {
    auto cfg = dynamic_cast<ControlFlowNode *>(node.get());
    if (cfg)
      cfg->generate_to(block);
    else
      block.push_back(std::move(node));
  }
  for (auto p : need_to_point_to_end)
    *p = block.size() - 1;
  if (pos_vec.size() > 1) {
    auto iwjt = static_cast<IfWithJmpTable *>(conditional_jmp);
    for (int i = 0; i < pos_vec.size(); i++)
      iwjt->jmp_tab.emplace_back(std::move(conditional_blocks[i].first), pos_vec[i]);
  } else {
    auto itj = static_cast<IfTrueJmp *>(conditional_jmp);
    itj->cond = std::move(conditional_blocks[0].first);
    itj->dest = pos_vec.front();
  }
  conditional_blocks.clear();
  else_block.clear();
}
std::string IfTrueJmp::to_string() const {
  return fmt::format("(if {} goto {} else {})", cond->to_string(), dest, false_dest);
}

TmpObjectPtr IfTrueJmp::eval() {
  auto val = cond->eval();
  if (val->is_true())
    Memory::pc = dest;
  else
    Memory::pc = false_dest;
  return nullptr;
}

void WhileStmt::generate_to(vector<AstNodePtr> &block) {
  /*
  while(cond)
  {
    stmts...
  }
  =>
cond_pos:
if cond loop_body_pos, end
loop_body_pos:
  stmts
  goto cond_pos
end:
*/

  int cond_pos = block.size() - 1;
  auto check_stmt = new IfTrueJmp;
  check_stmt->cond = std::move(condition);

  block.push_back(AstNodePtr(check_stmt));
  int loop_body_pos = block.size() - 1;
  check_stmt->dest = loop_body_pos;

  for (auto &node : loop_body) {
    if (auto cfg = dynamic_cast<ControlFlowNode *>(node.get()))
      cfg->generate_to(block);
    else
      block.emplace_back(std::move(node));
  }
  block.push_back(std::make_unique<Goto>(cond_pos));
  int end_pos = block.size() - 1;
  check_stmt->false_dest = end_pos;

  // clear
  loop_body.clear();
}
} // namespace cake