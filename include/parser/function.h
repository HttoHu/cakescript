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
  int param_size = 0;
  std::vector<AstNodePtr> block;

  void add_use(CallNode *node) { use_list.push_back(node); }
  void gen_func_object();
  std::string_view get_func_name() { return func_name.text; }

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
  CallNode(Token _func_name, std::vector<AstNodePtr> _args) : func_name(_func_name), args(std::move(_args)) {}
  CallNode(Token _func_name, ObjectBase *callable, std::vector<AstNodePtr> _args)
      : func_name(_func_name), executor(callable), args(std::move(_args)) {}

  TmpObjectPtr eval() override;
  std::string to_string() const override;
  std::string_view get_func_name() { return func_name.text; }

private:
  friend class SymbolTable;
  union {
    FunctionDef *def;
    ObjectBase *executor;
  };
  friend class FunctionDef;
  Token func_name;
  std::vector<AstNodePtr> args;
};

class CallMemberFunction : public AstNode {
public:
  CallMemberFunction(AstNodePtr _gen_func, AstNode *_this, std::vector<AstNodePtr> _args)
      : gen_func_node(std::move(_gen_func)), object_this(_this), args(std::move(_args)) {}
  TmpObjectPtr eval() override;

private:
  AstNodePtr gen_func_node;
  // we need append object_this at the end of the function
  AstNode *object_this;
  std::vector<AstNodePtr> args;
};
class RetNode : public AstNode {
public:
  RetNode(AstNodePtr _expr) : expr(std::move(_expr)) {}
  TmpObjectPtr eval() override;
  std::string to_string() const override { return "(return " + expr->to_string() + ")"; }

private:
  AstNodePtr expr;
};
} // namespace cake