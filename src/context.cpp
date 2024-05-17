#include <context.h>
#include <parser/symbol.h>
#include <runtime/intern_function.h>
namespace cake {
Context::Context() {
  // all tokens have default file.
  source_file_list.push_back("unknown file!");
  sym_tab = new SymbolTable;
}

Context *Context::global_context() {
  static Context *ret;

  if (!ret) {
    ret = new Context();
    inter_funcs::reg_func(*ret->sym_tab, "print", inter_funcs::print);
    inter_funcs::reg_func(*ret->sym_tab, "length", inter_funcs::length);
    return ret;
  }
  return ret;
}
SymbolTable *Context::global_symtab() { return global_context()->sym_tab; }

void Context::clear() {
  sym_tab->clear();
  sym_tab->new_block();
  source_file_list.resize(1);
}
size_t Context::cblk_vcnt() const { return sym_tab->cfunc_vcnt(); }

void Context::run() {
  for (auto &stmt : global_stmts) {
    stmt->eval_no_value();
  }
}

void Context::set_global_stmts(std::vector<AstNodePtr> stmts) { global_stmts = std::move(stmts); }
} // namespace cake