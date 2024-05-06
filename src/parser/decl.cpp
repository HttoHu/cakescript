#include <context.h>
#include <fmt/format.h>
#include <parser/decl.h>
#include <parser/symbol.h>

namespace cake {
AstNodePtr Parser::parse_decl() {
  switch (peek(0).kind) {
  case TokenKind::LET: {
    match(TokenKind::LET);
    auto ret = std::make_unique<VarDecl>();
    while (true) {
      auto sym = match(TokenKind::IDENTIFIER);
      VarDecl::VarDeclUnit vu(sym);
      if (peek(0).kind == TokenKind::ASSIGN) {
        match(TokenKind::ASSIGN);
        auto expr = parse_expr();
        vu.init_expr = std::move(expr);
      }
      vu.stac_index = Context::global_symtab()->cblk_vcnt();
      // add this var to symbol table
      Context::global_symtab()->add_symbol(sym.text, new VarSymbol(vu.stac_index, sym.text));
      ret->add_unit(std::move(vu));
      if (peek(0).kind != COMMA)
        break;
      match(COMMA);
    }
    return ret;
  }
  case TokenKind::FUNCTION:
    unreachable();
  default:
    unreachable();
  }
}

std::string VarDecl::to_string() const {
  std::string ret = "(vardecl ";
  for (auto &[name, init, stac_index] : var_decls) {
    if (init)
      ret += fmt::format("({} {} {})", name.text, init->to_string(), stac_index);
    else
      ret += fmt::format("({} {})", name.text, stac_index);
  }
  ret += ')';
  return ret;
}
} // namespace cake