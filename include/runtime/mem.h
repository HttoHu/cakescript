#pragma once
#include <cinttypes>
#include <vector>
#include <stdexcept>
namespace cake {
using byte = uint8_t;
class ObjectBase;

class Memory {
public:
  Memory();
  static Memory gmem;
  static int pc;
  void new_func(int blk_size);
  void end_func();
  ObjectBase *&get_local(int offset) {
    if (offset >= block_size_vec.back())
      throw std::runtime_error("range error!");
    return pool[sp - offset];
  }
  void clear();
  void print_status();
  void set_ret(ObjectBase *ret);
  ObjectBase *func_ret = nullptr;

private:
  std::vector<ObjectBase *> pool;
  std::vector<int> block_size_vec;
  // to store the pc of every function frame
  std::vector<int> pcs;

  int sp = -1;
};

} // namespace cake