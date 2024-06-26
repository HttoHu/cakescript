#pragma once
#include <basic/tmp_ptr.h>
#include <lexer.h>
#include <memory>
#include <runtime/object.h>
#include <utils.h>
namespace cake {
using TmpObjectPtr = basic::TmpPtr<ObjectBase>;
class AstNode {
public:
  virtual ~AstNode() = default;
  // return an object, note that the eval is only to view can't be delete
  virtual TmpObjectPtr eval() { unreachable(); };
  // return a value that caller should manage.
  virtual ObjectBase *eval_with_create() { return eval()->clone(); }
  // only to execute some nodes, no value returned.
  virtual void eval_no_value() { eval(); }
  virtual std::string to_string() const { return "unkonwn ast node"; }
  virtual bool left_value() const { return false; }
  // if the node is left value, it can get the address of the ObjectBase*
  virtual ObjectBase **get_left_val() { return nullptr; }

private:
};
// to present the object of return;
class EmptyNode : public AstNode {
public:
  TmpObjectPtr eval() override { return TmpObjectPtr(eval_with_create(),true); }
  ObjectBase* eval_with_create()override{return new NullObject;}
  std::string to_string() const override { return "(empty)"; }

private:
};
using AstNodePtr = std::unique_ptr<AstNode>;

} // namespace cake