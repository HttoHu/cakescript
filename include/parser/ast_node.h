#pragma once
#include <lexer.h>
#include <memory>
#include <object.h>
#include <utils.h>
namespace cake {
class AstNode {
public:
  virtual ~AstNode() = default;
  virtual ObjectBase *eval() { unreachable(); };
  virtual std::string to_string() const { return "unkonwn ast node"; }
  virtual bool need_delete_eval_object() const { return false; }
  virtual bool left_value() const { return false; }
  // if the node is left value, it can get the address of the ObjectBase*
  virtual ObjectBase **get_left_val() { return nullptr; }

private:
};

using AstNodePtr = std::unique_ptr<AstNode>;

} // namespace cake