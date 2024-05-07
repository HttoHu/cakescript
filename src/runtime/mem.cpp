#include <object.h>
#include <runtime/mem.h>

namespace cake {
Memory::Memory() : block_size_vec({0}) {}
Memory Memory::gmem;
void Memory::new_block(int blk_size) {
  sp += block_size_vec.back();
  block_size_vec.push_back(blk_size);
  pool.resize(sp + blk_size);
  for (int i = 0; i < blk_size; i++)
    pool[i] = nullptr;
}

void Memory::end_block() {
  int sz = block_size_vec.back();
  block_size_vec.pop_back();

  while (sz) {
    delete pool[sp + sz - 1];
    sz--;
  }
  pool.resize(sp);
  sp -= block_size_vec.back();
}

void Memory::clear(){
  
}
} // namespace cake