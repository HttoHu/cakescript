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
    inter_funcs::reg_func(*ret->sym_tab, "print",inter_funcs::print);
    return ret;
  }
  return ret;
}
SymbolTable *Context::global_symtab() { return global_context()->sym_tab; }

void Context::clear() {
  sym_tab->clear();
  source_file_list.resize(1);
}
size_t Context::cblk_vcnt() const { return sym_tab->cfunc_vcnt(); }

void Context::run() {
  for (auto &stmt : global_stmts) {
    if (stmt->need_delete_eval_object())
      delete stmt->eval();
    else
      stmt->eval();
  }
}

void Context::set_global_stmts(std::vector<AstNodePtr> stmts) { global_stmts = std::move(stmts); }
} // namespace cake