#pragma once
#include <lexer.h>
#include <parser/ast_node.h>

namespace cake {
class Parser {
public:
  Parser(Scanner scanner) : lexer(std::move(scanner)) {}
  Token match(TokenKind kind);
  Token peek(int sz) { return lexer.peek(sz); }

  // current token tag.
  TokenKind ctok_kind() { return peek(0).kind; }

  std::vector<AstNodePtr> parse_stmts();

  AstNodePtr parse_stmt();
  AstNodePtr parse_expr() { return parse_expr_imp(16); }
  [[noreturn]] void syntax_error(const std::string &error_info);
  [[noreturn]] void syntax_error(const std::string &error_info, Token tok);

private:
  Scanner lexer;
  AstNodePtr parse_unit();
  AstNodePtr parse_binary();
  AstNodePtr parse_expr_imp(int pred);

  AstNodePtr parse_decl();
};
} // namespace cake