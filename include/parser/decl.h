#pragma once
#include <parser/parser.h>
namespace cake {
// begin with let
class VarDecl : public AstNode {
public:
  struct VarDeclUnit {
    VarDeclUnit(Token _name) : varname(_name){}

    Token varname;
    // if init_expr is empty,then the value is nullptr;
    AstNodePtr init_expr = nullptr;
    size_t stac_index;
  };
  void add_unit(VarDeclUnit vu) { var_decls.push_back(std::move(vu)); }
  
  std::string to_string()const override;
private:
  std::vector<VarDeclUnit> var_decls;
};

class FunctionDecl : public AstNode {};
} // namespace cake