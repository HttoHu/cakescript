#pragma once
#include <string>
#include <vector>

namespace cake {
using std::string;
using std::vector;
class SymbolTable;

class Context {
public:
  Context();

  // return added file index
  size_t add_source_file(const std::string &file) {
    source_file_list.push_back(file);
    return source_file_list.size() - 1;
  }
  std::string get_source_file(int idx) const { return source_file_list[idx]; }

  static Context *global_context();
  static SymbolTable* global_symtab();

  void clear();
  size_t cblk_vcnt()const;
private:
  vector<string> source_file_list;
  SymbolTable* sym_tab;
};
} // namespace cake