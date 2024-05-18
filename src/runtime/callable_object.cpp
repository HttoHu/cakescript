#include <runtime/callable_object.h>
#include <runtime/mem.h>

namespace cake {
ObjectBase *FunctionObject::apply(std::vector<ObjectBase *> args) {
  // plus 1 to store return value.
  Memory::gmem.new_func(frame_size);
  for (int i = 0; i < args.size(); i++)
    Memory::gmem.get_local(i) = args[i]->clone();
  for (Memory::pc = 0; Memory::pc < insts.size(); Memory::pc++) {
    insts[Memory::pc]->eval_no_value();
  }
  Memory::gmem.end_func();
  auto ret = Memory::gmem.func_ret;
  return ret;
}
} // namespace cake