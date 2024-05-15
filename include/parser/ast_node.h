#pragma once
#include <lexer.h>
#include <memory>
#include <runtime/object.h>
#include <utils.h>
namespace cake {
class AstNode {
public:
  virtual ~AstNode() = default;
  virtual ObjectBase *eval() { unreachable(); };
  // return a value that user should manage.
  virtual ObjectBase *eval_with_create() { return eval(); }
  virtual std::string to_string() const { return "unkonwn ast node"; }
  virtual bool need_delete_eval_object() const { return false; }
  virtual bool left_value() const { return false; }
  // if the node is left value, it can get the address of the ObjectBase*
  virtual ObjectBase **get_left_val() { return nullptr; }

private:
};
class EmptyNode : public AstNode {
public:
  ObjectBase *eval() override { return nullptr; }
  std::string to_string() const override { return "(empty)"; }

private:
};
using AstNodePtr = std::unique_ptr<AstNode>;

} // namespace cake