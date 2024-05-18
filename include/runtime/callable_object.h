#pragma once

#include <parser/ast_node.h>
#include <runtime/object.h>

namespace cake {
// user defined function
class FunctionObject : public ObjectBase {
public:
  FunctionObject(int f_size, int _arg_count,std::vector<AstNodePtr> _insts) : frame_size(f_size),arg_count(_arg_count), insts(std::move(_insts)) {}
  ObjectBase *apply(std::vector<ObjectBase*> args) override;
private:
  int frame_size;
  int arg_count;
  // make sure that insts already flatten
  std::vector<AstNodePtr> insts;
};
} // namespace cake