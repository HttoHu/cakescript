#pragma once
#include <memory>
#include <string>
#include <vector>
namespace cake {
using std::string;
using std::vector;
class SymbolTable;
class AstNode;
using AstNodePtr = std::unique_ptr<AstNode>;

class Context {
public:
  Context();

  // return added file index
  size_t add_source_file(const std::string &file) {
    source_file_list.push_back(file);
    return source_file_list.size() - 1;
  }
  void add_init_expr(AstNodePtr stmt);
  std::string get_source_file(int idx) const { return source_file_list[idx]; }

  static Context *global_context();
  static SymbolTable *global_symtab();

  void clear();
  size_t cblk_vcnt() const;
  void set_global_stmts(std::vector<AstNodePtr> stmts);
  void run();

private:
  vector<string> source_file_list;
  SymbolTable *sym_tab;
  std::vector<AstNodePtr> init_stmts;
  std::vector<AstNodePtr> global_stmts;
};
} // namespace cake