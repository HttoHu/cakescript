#include <fmt/format.h>
#include <parser/function.h>
#include <parser/symbol.h>
namespace cake {
void SymbolTable::fill_undefined_func_nodes(FunctionDef *func_def) {
  // for (auto &node : call_without_func_def) {
  //   std::cout << "@" << node.second->get_func_name() << " ";
  //   for (auto i : node.first) {
  //     std::cout << i << " ";
  //   }
  //   std::cout << std::endl;
  // }
  auto it = call_without_func_def.lower_bound(cur_block_seqs);
  // to test if a is the prefix of b
  auto is_prefix_of = [](const std::vector<int> &a, const std::vector<int> &b) {
    if (a.size() > b.size())
      return false;
    for (std::vector<int>::size_type i = 0; i < a.size(); i++)
      if (a[i] != b[i])
        return false;
    return true;
  };
  while (it != call_without_func_def.end()) {
    if (is_prefix_of(cur_block_seqs, it->first)) {
      if (it->second->get_func_name() == func_def->get_func_name()) {
        func_def->add_use(it->second);
        it = call_without_func_def.erase(it);
      } else
        it++;
    } else
      break;
  }
}
} // namespace cake