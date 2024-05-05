#pragma once

#include <lexer.h>
#include <map>
#include <parser/ast_node.h>
namespace cake {
using std::deque;
using std::map;
enum SymbolKind { SYM_VAR, SYM_FUNC, SYM_CLASS };

/*
  a symbol which maintain a simple use-def chain.
  using this simple use-def chain, we can rewrite every use or we can put defination of a
  function after its use.
*/
class Symbol {
public:
  using iterator = std::vector<AstNode *>::iterator;

  Symbol(SymbolKind _kind, std::string_view _name, AstNode *_def = nullptr)
      : kind(_kind), name(_name), def_node(_def) {}

  std::string_view get_name() const { return name; }
  SymbolKind get_kind() const { return kind; }
  const AstNode *get_def_node() const { return def_node; }

  iterator begin() { return use_list.begin(); }
  iterator end() { return use_list.end(); }
  size_t use_size() const { return use_list.size(); }
  void add_use(AstNode *node) { use_list.push_back(node); }

  virtual ~Symbol() {}

private:
  SymbolKind kind;
  std::string_view name;
  // vistor ,don't hold ownership
  AstNode *def_node;
  // ditto
  std::vector<AstNode *> use_list;
};

class SymbolTable {
public:
  SymbolTable() : symbol_table(1) {}
  void new_block() { symbol_table.push_back({}); }
  void end_block() {
    auto &mp = symbol_table.back();
    for (auto &[key, val] : mp)
      delete val;
    symbol_table.pop_back();
  }

  // if return nullptr, the symbol not found.
  Symbol *find_symbol(string_view name) {
    for (auto block = symbol_table.rbegin(); block != symbol_table.rend(); block++) {
      auto it = block->find(name);
      if (it != block->end())
        return it->second;
    }
    return nullptr;
  }
  void add_symbol(string_view name, Symbol *sym) { symbol_table.back().insert({name, sym}); }
  // current block variable count.
  size_t cblk_vcnt() {
    size_t ret = 0;
    auto &mp = symbol_table.back();
    for (auto &[name, sym] : mp) {
      if (sym->get_kind() == SymbolKind::SYM_VAR)
        ret++;
    }
    return ret;
  }
  void clear() {
    while (!symbol_table.empty())
      end_block();
    symbol_table.resize(1);
  }

private:
  deque<map<string_view, Symbol *>> symbol_table;
};

} // namespace cake