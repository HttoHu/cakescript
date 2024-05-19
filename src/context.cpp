#include <context.h>
#include <lib/io.h>
#include <parser/symbol.h>
#include <runtime/intern_function.h>
#include <runtime/mem.h>
namespace cake {
Context::Context() {
  // all tokens have default file.
  source_file_list.push_back("unknown file!");
  sym_tab = new SymbolTable;
}
void Context::add_init_expr(AstNodePtr stmt) { init_stmts.push_back(std::move(stmt)); }
Context *Context::global_context() {
  static Context *ret;
  if (!ret) {
    ret = new Context();
    inter_funcs::reg_func(*ret->sym_tab, "print", inter_funcs::print);
    inter_funcs::reg_func(*ret->sym_tab, "length", inter_funcs::length);
    lib::import_console(ret);
    ret->sym_tab->new_block();
    return ret;
  }
  return ret;
}
SymbolTable *Context::global_symtab() { return global_context()->sym_tab; }

void Context::clear() {
  sym_tab->clear();
  sym_tab->new_block();
  sym_tab->cfunc_vcnt() = init_stmts.size();
  source_file_list.resize(1);
  global_stmts.clear();
}
size_t Context::cblk_vcnt() const { return sym_tab->cfunc_vcnt(); }

void Context::run() {
  Memory::gmem.new_func(cake::Context::global_context()->cblk_vcnt());
  for (auto &it : init_stmts)
    it->eval_no_value();
  for (Memory::pc = 0; Memory::pc < global_stmts.size(); Memory::pc++) {
    global_stmts[Memory::pc]->eval();
  }
  Memory::gmem.end_func();
  clear();
  Memory::gmem.clear();
  Memory::pc = 0;
}
void Context::dump_stmts() const {
  for (auto &it : init_stmts)
    std::cout << it->to_string() << std::endl;
  for (auto &it : global_stmts) {
    std::cout << it->to_string() << std::endl;
  }
}
void Context::set_global_stmts(std::vector<AstNodePtr> stmts) { global_stmts = std::move(stmts); }
} // namespace cake