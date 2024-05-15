#include <iostream>
#include <runtime/mem.h>
#include <runtime/object.h>

namespace cake {
Memory::Memory() : block_size_vec(1) {}
Memory Memory::gmem;
int Memory::pc = 0;

void Memory::new_func(int blk_size) {
  pcs.push_back(pc);
  sp += blk_size;
  block_size_vec.push_back(blk_size);
  pool.resize(sp + 1);
}
void Memory::print_status() {
  std::cout << "1. pcs: ";
  for (auto p : pcs)
    std::cout << p << " ";
  std::cout << "\n2. blk size: ";
  for (auto b : block_size_vec)
    std::cout << b << " ";
  std::cout << std::endl;
  std::cout << "3. sp:" << sp << std::endl;
  std::cout << "4. cur_stac " << std::endl;

  for (int i = 0; i <= sp; i++) {
    if (pool[i])
      std::cout << "#" << i << "\t: " << pool[i]->to_string() << ":" << pool[i] << std::endl;
    else
      std::cout << "#" << i << "\t: null" << std::endl;
  }
}

void Memory::set_ret(ObjectBase *ret) { get_local(block_size_vec.back() - 1) = ret; }
void Memory::end_func() {
  int sz = block_size_vec.back();
  block_size_vec.pop_back();

  while (sz) {
    delete pool[sp];
    sp--;
    sz--;
  }
  pool.resize(sp + 1);
  pc = pcs.back();
  pcs.pop_back();
}

void Memory::clear() {
  for (auto it : pool)
    if (it)
      delete it;
  pool.clear();
  pcs.clear();
  sp = 0;
  block_size_vec.resize(1);
  block_size_vec.back() = 0;
}
} // namespace cake