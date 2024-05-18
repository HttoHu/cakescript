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
    return new IntegerObject((int64_t)arr->get_array_length());
  }
  cake_runtime_error("length() expect an array object " + std::to_string(args.size()));
}

ObjectBase *_array_push(std::vector<ObjectBase *> args) {
  if (args.size() != 2)
    cake_runtime_error("Array.push(item): expect one argument! ");
  static_cast<ArrayObject *>(args[1])->push_obj(args[0]->clone());
  return new NullObject;
}

ObjectBase *_array_pop(std::vector<ObjectBase *> args) {
  if (args.size() != 1)
    cake_runtime_error("Array.pop(): no need argument! ");
  static_cast<ArrayObject *>(args[0])->pop();
  return new NullObject;
}
} // namespace cake::inter_funcs