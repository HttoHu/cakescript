#include <context.h>
#include <fmt/format.h>
#include <parser/function.h>
#include <parser/loop_branch.h>
#include <parser/parser.h>
#include <parser/symbol.h>
#include <filesystem>

namespace cake {
Token Parser::match(TokenKind kind) {
  auto tok = lexer.next_token();
  if (tok.kind != kind)
    lexer.error(tok, fmt::format("unexpected token {}", tok.text));
  return tok;
}

std::vector<AstNodePtr> Parser::parse_stmts() {
  std::vector<AstNodePtr> ret;
  while (peek(0).kind != NIL && peek(0).kind != END) {
    ret.emplace_back(parse_stmt());
    if (peek(0).kind == SEMI)
      match(SEMI);
  }
  return ret;
}

std::vector<AstNodePtr> Parser::parse_global() {
  std::vector<AstNodePtr> stmts = parse_stmts();
  std::vector<AstNodePtr> ret;
  for (auto &node : stmts) {
    if (auto cfg = dynamic_cast<ControlFlowNode *>(node.get()))
      cfg->generate_to(ret);
    else if (auto func_def = dynamic_cast<FunctionDef *>(node.get())) {
      func_def->gen_func_object();
    } else
      ret.emplace_back(std::move(node));
  }
  return ret;
}

std::vector<AstNodePtr> Parser::parse_block() {
  Context::global_symtab()->new_block();
  bool have_begin = false;
  if (peek(0).kind == BEGIN) {
    match(BEGIN);
    auto ret = parse_stmts();
    match(END);
    Context::global_symtab()->end_block();
    return ret;
  } else {
    auto stmt = parse_stmt();
    Context::global_symtab()->end_block();
    std::vector<AstNodePtr> ret;
    ret.push_back(std::move(stmt));
    return ret;
  }
}

AstNodePtr Parser::parse_stmt() {
  AstNodePtr ret;
  switch (peek(0).kind) {
  case FUNCTION:
  case LET:
    ret = parse_decl();
    break;
  case BREAK:
    lexer.next_token();
    return std::make_unique<Goto>(TokenKind::BREAK);
  case CONTINUE:
    lexer.next_token();
    return std::make_unique<Goto>(TokenKind::CONTINUE);
  case INC:
  case DEC:
  case MINUS:
  case LPAR:
  case IDENTIFIER:
  case INTEGER:
    ret = parse_expr();
    break;
  case IF:
    return parse_if();
  case FOR:
    return parse_for();
  case WHILE:
    return parse_while();
  case RETURN: {
    match(RETURN);
    if (peek(0).kind != NIL && peek(0).kind != END && peek(0).kind != SEMI)
      return make_unique<RetNode>(parse_expr());
    return std::make_unique<RetNode>(std::make_unique<EmptyNode>());
  }
  default:
    syntax_error("unexpected token " + std::string{peek(0).text});
  }
  if (peek(0).kind == SEMI)
    match(SEMI);
  return ret;
}

void Parser::syntax_error(const std::string &error_info) {
  throw std::runtime_error(fmt::format("syntax error: {}:{} ", peek(0).get_file_pos(), error_info));
}

void Parser::syntax_error(const std::string &error_info, Token tok) {
  using namespace std::filesystem;
  std::string abs_path = absolute(path(tok.get_file_pos()));
  throw std::runtime_error(fmt::format("syntax error: {}: {} ",abs_path , error_info));
}
} // namespace cake