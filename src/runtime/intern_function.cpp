#include <iostream>
#include <parser/symbol.h>
#include <runtime/intern_function.h>
namespace cake::inter_funcs {

void reg_func(SymbolTable &sym_tab, std::string_view func_name, InternalFunction::FuncTy func) {
  sym_tab.add_global_symbol(func_name, new FunctionSymbol(std::string{func_name}, new InternalFunction(func)));
}

ObjectBase *print(std::vector<ObjectBase *> args) {
  for (auto arg : args)
    std::cout << arg->to_string();
  std::cout << std::endl;
  return new NullObject();
}
} // namespace cake::inter_funcs