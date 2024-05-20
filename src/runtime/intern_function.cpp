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

ObjectBase *_array_resize(std::vector<ObjectBase *> args) {
  if (args.size() != 2)
    cake_runtime_error("Array.resize(): expect one args ");
  auto newSz = IntegerObject::get_integer(args.front());
  static_cast<ArrayObject *>(args.back())->resize(newSz);
  return new NullObject;
}

ObjectBase *_string_set(std::vector<ObjectBase *> args) {
  if (args.size() != 3)
    cake_runtime_error("String.set expect 2 args");
  int idx = IntegerObject::get_integer(args[0]);
  std::string &val = StringObject::get_str_from_obj(args[1]);
  static_cast<StringObject *>(args[2])->str[idx] = val[0];
  return new NullObject;
}
ObjectBase *_string_trim(std::vector<ObjectBase *> args) {
  if (args.size() != 1)
    cake_runtime_error("String.trim expected no args");
  std::string val = StringObject::get_str_from_obj(args[0]);
  int l = 0, r = val.size() - 1;
  while (l < r && std::isspace(val[l]))
    l++;
  while (l < r && std::isspace(val[r]))
    r--;
  return new StringObject(val.substr(l, r - l));
}
ObjectBase *_string_index_of(std::vector<ObjectBase *> args) {
  if (args.size() < 2 || args.size() > 3)
    cake_runtime_error("String.indexOf expected 1 arg");
  std::string &val1 = StringObject::get_str_from_obj(args.back());
  std::string &val2 = StringObject::get_str_from_obj(args[0]);
  if (args.size() == 3) {
    int64_t pos = IntegerObject::get_integer(args[1]);
    return new IntegerObject((int64_t)val1.find(val2, pos));
  }
  return new IntegerObject((int64_t)val1.find(val2));
}
} // namespace cake::inter_funcs