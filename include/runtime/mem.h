#pragma once
#include <cassert>
#include <cinttypes>
#include <stdexcept>
#include <vector>

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
    assert(offset < block_size_vec.back() && "Memory range error");
    return pool[sp - offset];
  }
  ObjectBase *&get_global(int offset) {
    assert(offset < pool.size() && "Memory range error");
    return pool[offset];
  }
  void clear();
  void print_status();
  ObjectBase *func_ret = nullptr;

private:
  std::vector<ObjectBase *> pool;
  std::vector<int> block_size_vec;
  // to store the pc of every function frame
  std::vector<int> pcs;

  int sp = -1;
};

} // namespace cake