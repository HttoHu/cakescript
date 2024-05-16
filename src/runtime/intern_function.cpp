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
ObjectBase *length(std::vector<ObjectBase *> args) {
  if (args.size() != 1)
    cake_runtime_error("call length error!, expected one arg but got " + std::to_string(args.size()));
  if (auto arr = dynamic_cast<ArrayObject *>(args[0])) {
    return new NumberObject((int64_t)arr->get_array_length());
  }
  cake_runtime_error("length() expect an array object " + std::to_string(args.size()));
}
} // namespace cake::inter_funcs