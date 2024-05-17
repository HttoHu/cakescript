#include <context.h>
#include <parser/function.h>
#include <parser/loop_branch.h>
#include <parser/parser.h>
#include <parser/symbol.h>
#include <runtime/callable_object.h>
#include <runtime/mem.h>
#include <runtime/object.h>

namespace cake {
std::vector<AstNodePtr> Parser::parse_expr_list(TokenKind begin, TokenKind end) {
  match(begin);
  std::vector<AstNodePtr> ret;
  while (peek(0).kind != end) {
    ret.push_back(parse_expr());
    if (peek(0).kind == TokenKind::COMMA)
      match(TokenKind::COMMA);
    else if (peek(0).kind != end)
      syntax_error("expected comma token!");
  }
  match(end);
  return ret;
}
ObjectBase *CallNode::eval() {
  std::vector<ObjectBase *> args_obj;
  args_obj.reserve(args.size());
  for (auto &it : args) {
    args_obj.emplace_back(it->eval());
  }
  auto ret = executor->apply(std::move(args_obj));
  return ret;
}
std::string CallNode::to_string() const {
  std::string ret = "(call " + std::string{func_name.text};
  for (auto &arg : args)
    ret += "," + arg->to_string();
  return ret + ")";
}

AstNodePtr Parser::parse_function_def() {
  match(TokenKind::FUNCTION);
  Context::global_symtab()->new_func();
  Token func_name = peek(0);
  match(IDENTIFIER);

  auto func_def = new FunctionDef(func_name);
  auto func_sym = new FunctionSymbol(std::string{func_name.text}, func_def);

  Context::global_symtab()->add_symbol(func_name.text, func_sym);

  // parse args
  match(LPAR);
  while (peek(0).kind != RPAR) {
    auto id = peek(0);
    match(IDENTIFIER);
    int idx = Context::global_symtab()->cfunc_vcnt();
    Context::global_symtab()->add_symbol(id.text, new VarSymbol(idx, id.text));
    if (peek(0).kind != RPAR)
      match(COMMA);
  }
  match(RPAR);
  match(BEGIN);
  func_def->block = parse_stmts();
  match(END);
  func_def->frame_size = Context::global_context()->cblk_vcnt();

  Context::global_symtab()->end_func();
  return AstNodePtr(func_def);
}

void FunctionDef::gen_func_object() {
  std::vector<AstNodePtr> insts;
  for (auto &node : block) {
    if (auto loop_branch = dynamic_cast<CFGNode *>(node.get())) {
      loop_branch->generate_to(insts);
    } else
      insts.emplace_back(std::move(node));
  }
  block.clear();

  func_obj = new FunctionObject(frame_size, std::move(insts));
  for (auto call_node : use_list)
    call_node->executor = func_obj;
}

ObjectBase *RetNode::eval() {
  Memory::gmem.func_ret = expr->eval_with_create();
  // set a very large number to jump out of function.
  Memory::gmem.pc = 0x3f3f3f3f;
  return nullptr;
}

} // namespace cake