#include <context.h>
#include <gtest/gtest.h>
#include <parser/decl.h>
#include <parser/loop_branch.h>
#include <runtime/mem.h>
using namespace cake;
cake::ObjectBase *get_var_val(std::string name);
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
  auto stmts = parser.parse_stmts();

  CFGNode::flatten_blocks(stmts);
  Memory::gmem.new_block(cake::Context::global_context()->cblk_vcnt());
  int i = 0;
  std::vector<int> seqs;
  std::vector<int> expected_seqs={0, 1, 4, 5, 6, 7, 10, 11, 14, 15};
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
}