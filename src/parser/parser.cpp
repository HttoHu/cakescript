#include <fmt/format.h>
#include <parser/parser.h>

namespace cake {
Token Parser::match(TokenKind kind) {
  auto tok = lexer.next_token();
  if (tok.kind != kind)
    lexer.error(tok, fmt::format("unexpected token {}", tok.text));
  return tok;
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
} // namespace cake