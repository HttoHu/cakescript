#include <context.h>
#include <parser/symbol.h>

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
    return ret;
  }
  return ret;
}
SymbolTable *Context::global_symtab() { return global_context()->sym_tab; }

void Context::clear() {
  sym_tab->clear();
  source_file_list.resize(1);
}
size_t Context::cblk_vcnt() const { return sym_tab->cblk_vcnt(); }
} // namespace cake