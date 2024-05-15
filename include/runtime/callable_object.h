#pragma once

#include <parser/ast_node.h>
#include <runtime/object.h>

namespace cake {
// user defined function
class FunctionObject : public ObjectBase {
public:
  FunctionObject(int f_size, std::vector<AstNodePtr> _insts) : frame_size(f_size), insts(std::move(_insts)) {}
  ObjectBase *apply(std::vector<ObjectBase*> args) override;

private:
  int frame_size;
  // make sure that insts already flatten
  std::vector<AstNodePtr> insts;
};
} // namespace cake