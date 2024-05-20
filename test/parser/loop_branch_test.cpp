#include <context.h>
#include <gtest/gtest.h>
#include <parser/decl.h>
#include <parser/loop_branch.h>
#include <runtime/mem.h>
#include <parser/symbol.h>
using namespace cake;
cake::ObjectBase *get_var_val(std::string name);

#ifndef DISABLE_UNIT
TEST(parserTest, LoopBranchTest1) {
  auto text = R"(
let a = 0,b = 1;
if(a)
  b=2;
else 
  b=3;
a=a+3;
let c = 0;
if(b==1)
  c=1;
else if(b==3)
{
  c=2;
  if(c!=2)
    c=a;
  else 
    c=c+13;
}
else if(b==2)
  c=31;
else 
  c=4;
)";
  cake::Scanner scanner(text);
  cake::Parser parser(std::move(scanner));
  cake::Context::global_symtab()->new_func();
  auto stmts = parser.parse_stmts();

  ControlFlowNode::flatten_blocks(stmts);
  Memory::gmem.new_func(cake::Context::global_context()->cblk_vcnt());
  int i = 0;
  std::vector<int> seqs;
  std::vector<int> expected_seqs = {0, 1, 4, 5, 6, 7, 10, 11, 14, 15};
  for (Memory::pc = 0; Memory::pc < stmts.size(); Memory::pc++) {
    seqs.push_back(Memory::pc);
    // std::cout << "#" << Memory::pc << "\tRUN" << stmts[Memory::pc]->to_string() << std::endl;
    stmts[Memory::pc]->eval();
  }
  // std::cout << n->to_string() << std::endl;
  EXPECT_EQ(get_var_val("b")->to_string(), "3");
  EXPECT_EQ(get_var_val("c")->to_string(), "15");

  EXPECT_EQ(seqs, expected_seqs);

  cake::Context::global_context()->clear();
  Memory::gmem.clear();
  Memory::pc = 0;
}

TEST(parserTest, LoopBranchTest2) {
  auto text = R"(
let sum = 0;
let i = 0;
while(i < 10){
  sum=i+sum;
  i=i+1;
}
)";
  cake::Scanner scanner(text);
  cake::Parser parser(std::move(scanner));
  Context::global_symtab()->new_func();
  auto stmts = parser.parse_stmts();
  Memory::gmem.new_func(cake::Context::global_context()->cblk_vcnt());
  auto while_node = dynamic_cast<LoopStmt *>(stmts[2].get());
  EXPECT_EQ(while_node->condition->to_string(), "(LT i(1) 10)");
  EXPECT_EQ(while_node->loop_body.size(), 2);
  EXPECT_EQ(while_node->loop_body[0]->to_string(), "(ASSIGN sum(0) (PLUS i(1) sum(0)))");
  EXPECT_EQ(while_node->loop_body[1]->to_string(), "(ASSIGN i(1) (PLUS i(1) 1))");
  ControlFlowNode::flatten_blocks(stmts);
  EXPECT_EQ(stmts[2]->to_string(), "(if (LT i(1) 10) goto 2 else 5)");
  EXPECT_EQ(stmts[3]->to_string(), "(ASSIGN sum(0) (PLUS i(1) sum(0)))");
  EXPECT_EQ(stmts[4]->to_string(), "(ASSIGN i(1) (PLUS i(1) 1))");
  EXPECT_EQ(stmts[5]->to_string(), "(goto 1)");

  for (; Memory::pc < stmts.size(); Memory::pc++) {
    // std::cout << "#" << Memory::pc << "\t:" << stmts[Memory::pc]->to_string() << std::endl;
    stmts[Memory::pc]->eval();
  }
  EXPECT_EQ(get_var_val("i")->to_string(), "10");
  EXPECT_EQ(get_var_val("sum")->to_string(), "45");
  cake::Context::global_context()->clear();
  Memory::gmem.clear();
  Memory::pc = 0;
}
TEST(parserTest, LoopBranchTest3) {
  auto text = R"(
let a = 1,b=100;
let sum = 0;
while (a < b) {
  sum = sum + a;
  a = a + 1;
}
)";
  cake::Scanner scanner(text);
  cake::Parser parser(std::move(scanner));
  cake::Context::global_symtab()->new_func();
  auto stmts = parser.parse_stmts();
  Memory::gmem.new_func(cake::Context::global_context()->cblk_vcnt());
  ControlFlowNode::flatten_blocks(stmts);

  for (; Memory::pc < stmts.size(); Memory::pc++) {
    stmts[Memory::pc]->eval();
  }
  EXPECT_EQ(get_var_val("sum")->to_string(), "4950");
  cake::Context::global_context()->clear();
  Memory::gmem.clear();
  Memory::pc = 0;
}
#endif
