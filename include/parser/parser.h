#pragma once
#include <lexer.h>
#include <parser/ast_node.h>

namespace cake {
class Parser {
public:
  Parser(Scanner scanner) : lexer(std::move(scanner)) {}
  void match(TokenKind kind);
  Token peek(int sz) { return lexer.peek(sz); }

  // current token tag.
  TokenKind ctok_kind() { return peek(0).kind; }
  AstNodePtr parse_expr(){return parse_expr_imp(4);}
private:
  Scanner lexer;

  AstNodePtr parse_unit();
  AstNodePtr parse_binary();
  AstNodePtr parse_expr_imp(int pred);
};
} // namespace cake