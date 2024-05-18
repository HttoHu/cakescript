#pragma once
#include <fmt/format.h>
#include <parser/parser.h>
#include <runtime/mem.h>

namespace cake {
struct VarDeclUnit {
  VarDeclUnit(Token _name) : varname(_name) {}

  Token varname;
  // if init_expr is empty,then the value is nullptr;
  AstNodePtr init_expr = nullptr;
  size_t stac_index;
};

// begin with let
template <bool IS_GLO> class VarDecl : public AstNode {
public:
  void add_unit(VarDeclUnit vu) { var_decls.push_back(std::move(vu)); }
  TmpObjectPtr eval() override {
    for (auto &unit : var_decls) {
      if constexpr (IS_GLO) {
        auto &ptr = Memory::gmem.get_global(unit.stac_index);
        if (ptr)
          delete ptr;
        ptr = unit.init_expr->eval_with_create();
      } else {
        auto &ptr = Memory::gmem.get_local(unit.stac_index);
        if (ptr)
          delete ptr;
        ptr = unit.init_expr->eval_with_create();
      }
    }
    return nullptr;
  }
  std::string to_string() const override {
    std::string ret = "(";
    if constexpr (IS_GLO)
      ret += "gvar_decl ";
    else
      ret += "var_decl";
    for (auto &[name, init, stac_index] : var_decls) {
      if (init)
        ret += fmt::format("({} {} {})", name.text, init->to_string(), stac_index);
      else
        ret += fmt::format("({} {})", name.text, stac_index);
    }
    ret += ')';
    return ret;
  }

private:
  std::vector<VarDeclUnit> var_decls;
};

class FunctionDecl : public AstNode {};
} // namespace cake