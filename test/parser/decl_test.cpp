#include <context.h>
#include <gtest/gtest.h>
#include <parser/decl.h>
#include <parser/symbol.h>
#include <runtime/mem.h>

using namespace cake;

static std::string parse_text(const std::string &str) {
  cake::Scanner scanner(str);
  cake::Parser parser(std::move(scanner));
  auto ret = parser.parse_stmt()->to_string();
  cake::Context::global_context()->clear();
  return ret;
}

static std::string parse_blocks(const std::string &str) {
  cake::Scanner scanner(str);
  cake::Parser parser(std::move(scanner));
  auto nodes = parser.parse_stmts();
  std::string ret;
  for (auto &node : nodes) {
    ret += node->to_string() + "\n";
  }
  cake::Context::global_context()->clear();
  return ret;
}
cake::ObjectBase *get_var_val(std::string name) {
  auto sym = Context::global_symtab()->find_symbol(name);
  if (!sym)
    return nullptr;
  auto var_sym = dynamic_cast<VarSymbol *>(sym);
  if (!var_sym)
    return nullptr;
  return Memory::gmem.get_global(var_sym->get_stac_pos());
}

#ifndef DISABLE_UNIT
TEST(parserTest, DeclTest1) {
  EXPECT_EQ(parse_text("let a=2,b=3,c=[1,2,3];"), "(gvar_decl (a 2 0)(b 3 1)(c (array [1,2,3]) 2))");
  EXPECT_EQ(parse_text("let b,c={a:123,b:[1,2,3]}"), "(gvar_decl (b 0)(c (object {a:123,b:(array [1,2,3])}) 1))");
  auto res = parse_blocks("let b,c={a:123,b:[1,2,3]};b=b+1;c=c+1;b=c=5+1;");
  EXPECT_NE(res.find("(ASSIGN glob b(0) (PLUS glob b(0) 1))"), std::string::npos);
  EXPECT_NE(res.find("(ASSIGN glob c(1) (PLUS glob c(1) 1))"), std::string::npos);
  EXPECT_NE(res.find("(ASSIGN glob b(0) (ASSIGN glob c(1) (PLUS 5 1)))"), std::string::npos);
  bool ok = false;
  try {
    auto res = parse_blocks("let b,c={a:123,b:[1,2,3]};\nb=b+1;\nc=c+1;\nb+1=5+1;");
  } catch (std::exception &e) {
    ok = true;
    EXPECT_EQ(std::string{e.what()},
              "syntax error: unknown file!:4:4:assign operation expect left value in the left side! ");
  }
  EXPECT_TRUE(ok);
  Context::global_context()->clear();
  Memory::gmem.clear();
}

TEST(parserTest, DeclTest2) {

  std::string text = R"(
let a = 34,b=1;
a=b+1;
b=a-3;
a=a+b*3;
let c = a;
c=b-3*a;
)";

  cake::Scanner scanner(text);
  cake::Parser parser(std::move(scanner));
  auto nodes = parser.parse_stmts();
  Memory::gmem.new_func(cake::Context::global_context()->cblk_vcnt());
  EXPECT_EQ(nodes[0]->eval().get(), nullptr);
  EXPECT_EQ(get_var_val("a")->to_string(), "34");
  EXPECT_EQ(get_var_val("b")->to_string(), "1");

  EXPECT_EQ(nodes[1]->eval()->to_string(), std::string{"2"});
  EXPECT_EQ(get_var_val("a")->to_string(), "2");
  EXPECT_EQ(get_var_val("b")->to_string(), "1");

  EXPECT_EQ(nodes[2]->eval()->to_string(), std::string{"-1"});
  EXPECT_EQ(nodes[3]->eval()->to_string(), std::string{"-1"});
  EXPECT_EQ(nodes[4]->eval().get(), nullptr);
  EXPECT_EQ(nodes[5]->eval()->to_string(), "2");
  EXPECT_EQ(get_var_val("a")->to_string(), "-1");
  EXPECT_EQ(get_var_val("b")->to_string(), "-1");
  EXPECT_EQ(get_var_val("c")->to_string(), "2");
  Context::global_context()->clear();
  Memory::gmem.clear();
}
#endif
