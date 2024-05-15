#include <context.h>
#include <map>
#include <parser/expr.h>
#include <parser/function.h>
#include <parser/parser.h>
#include <parser/symbol.h>
#include <runtime/mem.h>
#include <utils.h>

namespace cake {
using std::make_unique;

AstNodePtr Parser::parse_expr_imp(int ppred) {
  if (ppred == 1)
    return parse_unit();
  // clang-format off
  // <kind, precedance, is left to right.
  static std::map<TokenKind, std::pair<int, bool>> pred_tab = {
      {TokenKind::PLUS, {4, true}},
      {TokenKind::MINUS, {4, true}},
      {TokenKind::MUL, {3, true}},
      {TokenKind::DIV, {3, true}},
      {TokenKind::LE,{9,true}},
      {TokenKind::LT,{9,true}},
      {TokenKind::GE,{9,true}},
      {TokenKind::GT,{9,true}},
      {TokenKind::EQ,{10,false}},
      {TokenKind::NE,{10,false}},
      {TokenKind::ASSIGN, {16,false} }
  };
  // clang-format on
  auto get_pred = [&](TokenKind tag) -> std::pair<int, bool> {
    auto it = pred_tab.find(tag);
    if (it == pred_tab.end())
      return {100, false};
    return it->second;
  };

  auto left = parse_expr_imp(ppred - 1);

  std::vector<AstNodePtr> nodes;
  std::vector<Token> ops;
  nodes.push_back(std::move(left));
  while (!lexer.reach_to_end() && get_pred(lexer.peek(0).kind).first == ppred) {
    auto op = peek(0);
    lexer.next_token();

    ops.push_back(op);
    auto right = parse_expr_imp(ppred - 1);
    nodes.emplace_back(std::move(right));
  }

  if (nodes.size() == 1)
    return std::move(nodes.front());
  // left to right
  if (get_pred(ops.back().kind).second) {
    AstNodePtr root = std::move(nodes.front());
    for (int i = 1; i < nodes.size(); i++)
      root = make_unique<BinOp>(std::move(root), ops[i - 1].kind, std::move(nodes[i]));
    return root;
  } else {
    // right to left
    AstNodePtr root = std::move(nodes.back());
    // the size of ops = nodes.size()-1
    for (int i = ops.size() - 1; i >= 0; i--) {
      if (ppred == 16) {
        if (!nodes[i]->left_value())
          syntax_error("assign operation expect left value in the left side!", ops[i]);
        else
          root = make_unique<AssignOp>(std::move(nodes[i]), ops[i].kind, std::move(root));
      } else
        root = make_unique<BinOp>(std::move(nodes[i]), ops[i].kind, std::move(root));
    }
    return root;
  }
}
AstNodePtr Parser::parse_unit() {
  auto tok = lexer.next_token();
  switch (tok.kind) {
  // parse array
  case TokenKind::LSB: {
    std::vector<AstNodePtr> nodes;
    while (!lexer.reach_to_end() && lexer.peek(0).kind != RSB) {
      nodes.push_back(parse_expr());
      if (peek(0).kind != RSB)
        match(COMMA);
    }
    match(RSB);
    return make_unique<ArrayNode>(std::move(nodes));
  }
  case TokenKind::IDENTIFIER: {
    auto sym = Context::global_symtab()->find_symbol(tok.text);
    if (!sym)
      syntax_error("undefined symbol " + std::string{tok.text}, tok);
    if (sym->get_kind() == SYM_VAR) {
      return make_unique<Variable>(tok, static_cast<VarSymbol *>(sym)->get_stac_pos());
    } else if (sym->get_kind() == SYM_FUNC) {
      auto func = FunctionSymbol::get_func_def(sym);
      auto args = parse_expr_list(LPAR, RPAR);
      AstNodePtr ret = std::make_unique<CallNode>(tok, func, std::move(args));
      return ret;
    } else if(sym->get_kind() == SYM_CALLBLE){
      auto callable = FunctionSymbol::get_callable(sym);
      auto args = parse_expr_list(LPAR, RPAR);
      AstNodePtr ret = std::make_unique<CallNode>(tok, callable, std::move(args));
      return ret;
    }
    syntax_error("unsupported symbol kind ");
    break;
  }
  // parse object
  case TokenKind::BEGIN: {
    std::map<std::string, AstNodePtr> object_list;
    while (!lexer.reach_to_end() && lexer.peek(0).kind != END) {
      auto key = lexer.peek(0);
      // have not supported string key yet.
      match(IDENTIFIER);
      match(COLON);
      auto val = parse_expr();
      object_list.insert({std::string{key.text}, std::move(val)});
      if (peek(0).kind != END)
        match(COMMA);
    }
    match(END);
    return make_unique<ObjectNode>(std::move(object_list));
  }
  case TokenKind::INTEGER:
    return make_unique<cake::Literal>(tok);
  case TokenKind::LPAR: {
    auto ret = parse_expr();
    match(TokenKind::RPAR);
    return ret;
  }
  default:
    unreachable();
  }
}

ObjectBase *BinOp::eval() {
  auto lval = left->eval(), rval = right->eval();
#define BIN_OP_MP(TAG, OP)                                                                                             \
  case TokenKind::TAG: {                                                                                               \
    return lval->OP(rval);                                                                                    \
  }
  switch (op) {
    BIN_OP_MP(PLUS, add)
    BIN_OP_MP(MUL, mul)
    BIN_OP_MP(MINUS, sub)
    BIN_OP_MP(DIV, div)
    BIN_OP_MP(EQ, eq)
    BIN_OP_MP(NE, ne)
    BIN_OP_MP(GE, ge)
    BIN_OP_MP(GT, gt)
    BIN_OP_MP(LE, le)
    BIN_OP_MP(LT, lt)
  default:
    abort();
  }
}

Literal::Literal(Token lit) {
  if (lit.kind == TokenKind::INTEGER) {
    result_tmp = new NumberObject((int64_t)std::stoi(std::string{lit.text}));
  } else
    unreachable();
}

ObjectBase *AssignOp::eval() {
  auto ret = right->eval();
  switch (op) {

  case ASSIGN:
    *left->get_left_val() = ret->clone();
    break;
  default:
    unreachable();
  }
  return ret;
}
std::string AssignOp::to_string() const {
  switch (op) {
  case ASSIGN:
    return "(ASSIGN " + left->to_string() + " " + right->to_string() + ")";
  default:
    return "(unknown assign op)";
  }
}
ObjectBase *Variable::eval() { return Memory::gmem.get_local(stac_pos); }

ObjectBase **Variable::get_left_val() { return &Memory::gmem.get_local(stac_pos); }
} // namespace cake