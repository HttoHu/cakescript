#pragma once
#include <parser/parser.h>

namespace cake {
using std::pair;
using std::vector;
class CFGNode : public AstNode {
public:
  // flatten AstNode to control flow insts with insertion of jumps and branches.
  virtual void generate_to(vector<AstNodePtr> &block) = 0;

  // erase if and loop stmts and convert into liner IR
  static void flatten_blocks(std::vector<AstNodePtr> &stmts);
  // CFGNode need to transfer
  ObjectBase *eval() override { abort(); }
};

class IfStmt : public CFGNode {
public:
  IfStmt() : conditional_blocks(0), else_block(0) {}
  void generate_to(vector<AstNodePtr> &block) override;
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

class WhileStmt : public CFGNode {
public:
  void generate_to(vector<AstNodePtr> &block) override;
  std::string to_string() const override { return "(unflatten while node)"; }
  AstNodePtr condition;
  std::vector<AstNodePtr> loop_body;
};

class IfWithJmpTable : public AstNode {
public:
  std::string to_string() const override;
  ObjectBase *eval() override;
  std::vector<pair<AstNodePtr, int>> jmp_tab;
  int false_dest;

private:
};
class IfTrueJmp : public AstNode {
public:
  std::string to_string() const override;
  ObjectBase *eval() override;
  AstNodePtr cond;
  int dest;
  int false_dest;
};
class Goto : public AstNode {
public:
  Goto()=default;
  Goto(int d):dest(d){}
  std::string to_string() const override { return "(goto " + std::to_string(dest) + ")"; }
  ObjectBase *eval() override;
  int dest;

private:
};

}; // namespace cake