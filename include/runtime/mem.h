#pragma once
#include <cinttypes>
#include <vector>

namespace cake {
using byte = uint8_t;
class ObjectBase;

class Memory {
public:
  Memory();
  static Memory gmem;

  void new_block(int blk_size);
  void end_block();
  ObjectBase *&get_local(int offset) { return pool[sp + offset]; }
  void clear();
private:
  std::vector<ObjectBase *> pool;
  std::vector<int> block_size_vec;
  int sp = 0;
};

} // namespace cake