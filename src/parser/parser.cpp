#include <fmt/format.h>
#include <parser/parser.h>

namespace cake {
void Parser::match(TokenKind kind) {
  auto tok = lexer.next_token();
  if (tok.kind != kind)
    lexer.error(tok, fmt::format("unexpected token {}", tok.text));
}


} // namespace cake