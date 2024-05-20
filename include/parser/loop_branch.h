#pragma once
#include <parser/parser.h>

namespace cake {
using std::pair;
using std::vector;
class Goto;
class ControlFlowNode : public AstNode {
public:
  // flatten AstNode to control flow insts with insertion of jumps and branches. return need to processed goto
  // (continue,break)
  virtual std::vector<Goto *> generate_to(vector<AstNodePtr> &block) = 0;

  // erase if and loop stmts and convert into liner IR
  static void flatten_blocks(std::vector<AstNodePtr> &stmts);
  // CFGNode need to transfer
  TmpObjectPtr eval() override { abort(); }
};

class IfStmt : public ControlFlowNode {
public:
  IfStmt() : conditional_blocks(0), else_block(0) {}
  std::vector<Goto *> generate_to(vector<AstNodePtr> &block) override;
  std::string to_string() const override { return "(unflatten if node)"; }
  /*
  if (cond) {blk}      |
  else if (cond) {blk} |->conditional_blocks
  else {blk} -> else_block
  using this structure to build IfWithJmpTable, which may be faster then nested if-else in interpretion mode.
  */
  vector<pair<AstNodePtr, vector<AstNodePtr>>> conditional_blocks;
  std::vector<AstNodePtr> else_block;
};

// mixed for and while
class LoopStmt : public ControlFlowNode {
public:
  std::vector<Goto *> generate_to(vector<AstNodePtr> &block) override;
  std::string to_string() const override { return "(unflatten while node)"; }
  AstNodePtr init = nullptr; // can be empty
  AstNodePtr step = nullptr;
  AstNodePtr condition;
  std::vector<AstNodePtr> loop_body;
};

class IfWithJmpTable : public AstNode {
public:
  std::string to_string() const override;
  TmpObjectPtr eval() override;
  std::vector<pair<AstNodePtr, int>> jmp_tab;
  int false_dest;

private:
};
class IfTrueJmp : public AstNode {
public:
  std::string to_string() const override;
  TmpObjectPtr eval() override;
  AstNodePtr cond;
  int dest;
  int false_dest;
};
class Goto : public AstNode {
public:
  Goto() : type(TokenKind::NIL) {}
  Goto(int d) : type(TokenKind::NIL), dest(d) {}
  // to support break and continue
  Goto(TokenKind kind) : type(kind) {}

  std::string to_string() const override { return "(goto " + std::to_string(dest) + ")"; }
  TmpObjectPtr eval() override;
  int dest;
  TokenKind get_kind() const { return type; }

private:
  friend class LoopStmt;
  TokenKind type;
};

}; // namespace cake