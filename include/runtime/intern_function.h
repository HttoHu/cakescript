/*
  some internal functions
*/

#pragma once
#include <functional>
#include <runtime/object.h>
#include <string>
namespace cake {
class InternalFunction : public ObjectBase {
public:
  using FuncTy = std::function<ObjectBase *(std::vector<ObjectBase *>)>;
  InternalFunction(FuncTy _func) : func(std::move(_func)) {}
  std::string to_string() const override { return "internal function object"; }
  ObjectBase *apply(std::vector<ObjectBase *> args) override {
    auto ret = func(std::move(args));
    // clear args
    for (auto arg : args)
      delete arg;
    return ret;
  }

private:
  FuncTy func;
};
class SymbolTable;
namespace inter_funcs {

void reg_func(SymbolTable &sym_tab, std::string_view func_name, InternalFunction::FuncTy func);
ObjectBase *print(std::vector<ObjectBase *> args);
ObjectBase *length(std::vector<ObjectBase *> args);
} // namespace inter_funcs
} // namespace cake