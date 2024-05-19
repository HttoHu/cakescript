#include <context.h>
#include <fmt/format.h>
#include <parser/decl.h>
#include <parser/symbol.h>
#include <runtime/mem.h>
namespace cake {
AstNodePtr Parser::parse_decl() {
  switch (peek(0).kind) {
  case TokenKind::LET: {
    match(TokenKind::LET);
    AstNode *ret;
    if (Context::global_symtab()->in_global_block())
      ret = new VarDecl<true>();
    else
      ret = new VarDecl<false>();

    while (true) {
      auto sym = match(TokenKind::IDENTIFIER);
      VarDeclUnit vu(sym);
      if (peek(0).kind == TokenKind::ASSIGN) {
        match(TokenKind::ASSIGN);
        auto expr = parse_expr();
        vu.init_expr = std::move(expr);
      }
      vu.stac_index = Context::global_symtab()->cfunc_vcnt();
      // add this var to symbol table
      Context::global_symtab()->add_symbol(sym.text, new VarSymbol(vu.stac_index, sym.text));
      if (Context::global_symtab()->in_global_block())
      {
        static_cast<VarDecl<true> *>(ret)->add_unit(std::move(vu));
      }
      else
        static_cast<VarDecl<false> *>(ret)->add_unit(std::move(vu));
      if (peek(0).kind != COMMA)
        break;
      match(COMMA);
    }
    return AstNodePtr(ret);
  }
  case TokenKind::FUNCTION:
    return parse_function_def();
  default:
    syntax_error("unexpected token ");
  }
}
} // namespace cake