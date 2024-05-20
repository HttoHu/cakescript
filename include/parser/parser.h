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
  std::vector<AstNodePtr> parse_global();
  std::vector<AstNodePtr> parse_stmts();
  std::vector<AstNodePtr> parse_block();
  AstNodePtr parse_stmt();
  AstNodePtr parse_expr() { return parse_expr_imp(16); }
  [[noreturn]] void syntax_error(const std::string &error_info);
  [[noreturn]] void syntax_error(const std::string &error_info, Token tok);
  AstNodePtr parse_function_def();
private:
  Scanner lexer;
  AstNodePtr parse_unit();
  // precedence : 3, ++,--,-
  AstNodePtr parse_unary();
  AstNodePtr parse_binary();
  AstNodePtr parse_expr_imp(int pred);

  AstNodePtr parse_decl();
  AstNodePtr parse_if();
  AstNodePtr parse_while();
  // comma seperated list
  std::vector<AstNodePtr> parse_expr_list(TokenKind begin, TokenKind end);
};
} // namespace cake