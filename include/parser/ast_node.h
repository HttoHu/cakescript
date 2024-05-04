#pragma once
#include <lexer.h>
#include <memory>
#include <object.h>

namespace cake {
class AstNode {
public:
  virtual ~AstNode() = default;
  virtual ObjectBase *eval() = 0;
  virtual std::string to_string() const { return "unkonwn ast node"; }
  virtual bool need_delete_eval_object() const { return false; }

private:
};

using AstNodePtr = std::unique_ptr<AstNode>;

} // namespace cake