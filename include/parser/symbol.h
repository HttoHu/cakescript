#pragma once

#include <lexer.h>
#include <map>
#include <parser/ast_node.h>
namespace cake {
using std::deque;
using std::map;
enum SymbolKind { SYM_VAR, SYM_FUNC, SYM_CALLBLE, SYM_CLASS };
class FunctionDef;
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

class VarSymbol : public Symbol {
public:
  VarSymbol(size_t _pos, std::string_view name) : Symbol(SYM_VAR, name, nullptr), stac_pos(_pos) {}
  size_t get_stac_pos() const { return stac_pos; }

private:
  size_t stac_pos;
};

class FunctionSymbol : public Symbol {
public:
  FunctionSymbol(const std::string &func_name, FunctionDef *def) : Symbol(SYM_FUNC, func_name), func_def(def) {}

  // the callable object is prepared (mainly internal function)
  FunctionSymbol(const std::string &func_name, ObjectBase *_callable)
      : Symbol(SYM_CALLBLE, func_name), callable(_callable) {}

  static FunctionDef *get_func_def(Symbol *sym) { return static_cast<FunctionSymbol *>(sym)->func_def; }
  static ObjectBase *get_callable(Symbol *sym) { return static_cast<FunctionSymbol *>(sym)->callable; }

private:
  FunctionDef *func_def = nullptr;
  ObjectBase *callable = nullptr;
};
class SymbolTable {
public:
  SymbolTable() : symbol_table(1), func_vcnt(1) {}
  void new_block() { symbol_table.push_back({}); }
  void end_block() {
    auto &mp = symbol_table.back();
    for (auto &[key, val] : mp)
      delete val;
    symbol_table.pop_back();
  }
  void new_func() {
    func_vcnt.push_back(0);
    new_block();
  }
  void end_func() { func_vcnt.pop_back(); }
  // if return nullptr, the symbol not found.
  Symbol *find_symbol(string_view name) {
    for (auto block = symbol_table.rbegin(); block != symbol_table.rend(); block++) {
      auto it = block->find(name);
      if (it != block->end())
        return it->second;
    }
    return nullptr;
  }

  void add_symbol(string_view name, Symbol *sym) {
    symbol_table.back().insert({name, sym});
    func_vcnt.back() += sym->get_kind() == SYM_VAR;
  }
  void add_global_symbol(string_view name, Symbol *sym) {
    symbol_table.front().insert({name, sym});
    func_vcnt.back() += sym->get_kind() == SYM_VAR;
  }
  // current block variable count.
  size_t cfunc_vcnt() { return func_vcnt.back(); }
  void clear() {
    while (!symbol_table.empty())
      end_block();
    symbol_table.resize(1);
    func_vcnt.resize(1);
    func_vcnt[0] = 0;
  }

private:
  // to record current function have how many variables. function may be nested in other functions.
  std::vector<size_t> func_vcnt;
  deque<map<string_view, Symbol *>> symbol_table;
};

} // namespace cake