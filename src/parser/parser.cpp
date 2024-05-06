#include <fmt/format.h>
#include <parser/parser.h>

namespace cake {
Token Parser::match(TokenKind kind) {
  auto tok = lexer.next_token();
  if (tok.kind != kind)
    lexer.error(tok, fmt::format("unexpected token {}", tok.text));
  return tok;
}
std::vector<AstNodePtr> Parser::parse_stmts(){
  std::vector<AstNodePtr> ret;
  while(peek(0).kind!=NIL && peek(0).kind!=END){
    ret.emplace_back(parse_stmt());
  }
  return ret;
}
AstNodePtr Parser::parse_stmt() {
  AstNodePtr ret;
  switch (peek(0).kind) {
  case FUNCTION:
  case LET:
    ret = parse_decl();
    break;
  case LPAR:
  case IDENTIFIER:
  case INTEGER:
    ret = parse_expr();
    break;
  default:
    unreachable();
  }
  if (peek(0).kind == SEMI)
    match(SEMI);
  return ret;
}

void Parser::syntax_error(const std::string &error_info) {
  throw std::runtime_error(fmt::format("syntax error: {}:{} ", peek(0).get_file_pos(), error_info));
}

void Parser::syntax_error(const std::string &error_info,Token tok) {
  throw std::runtime_error(fmt::format("syntax error: {}:{} ", tok.get_file_pos(), error_info));
}
} // namespace cake