#pragma once

#include <parser/ast_node.h>
namespace cake {
class FunctionDef;
class FunctionObject;
class CallNode;
class FunctionDef : public AstNode {
public:
  FunctionDef(Token _name) : func_name(_name) {}

  int frame_size = 0;
  std::vector<AstNodePtr> params;
  std::vector<AstNodePtr> block;

  void add_use(CallNode *node) { use_list.push_back(node); }
  void gen_func_object();

private:
  Token func_name;
  std::vector<CallNode *> use_list;
  FunctionObject *func_obj = nullptr;
};

class CallNode : public AstNode {
public:
  CallNode(Token _func_name, FunctionDef *_def, std::vector<AstNodePtr> _args)
      : func_name(_func_name), def(_def), args(std::move(_args)) {
    def->add_use(this);
  }
  ObjectBase *eval() override;
  std::string to_string() const override;

private:
  union {
    FunctionDef *def;
    ObjectBase *executor;
  };
  friend class FunctionDef;
  Token func_name;
  std::vector<AstNodePtr> args;
};

class RetNode : public AstNode {
public:
  RetNode(AstNodePtr _expr) : expr(std::move(_expr)) {}
  ObjectBase *eval() override;
  std::string to_string() const override{ return "(return " + expr->to_string() + ")"; }

private:
  AstNodePtr expr;
};
} // namespace cake