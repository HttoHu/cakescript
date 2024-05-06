#include <context.h>
#include <gtest/gtest.h>
#include <parser/decl.h>
static std::string parse_text(const std::string &str) {
  cake::Scanner scanner(str);
  cake::Parser parser(std::move(scanner));
  auto node = parser.parse_stmt();
  cake::Context::global_context()->clear();
  return node->to_string();
}

static std::string parse_blocks(const std::string &str) {
  cake::Scanner scanner(str);
  cake::Parser parser(std::move(scanner));
  auto nodes = parser.parse_stmts();
  cake::Context::global_context()->clear();
  std::string ret;
  for (auto &node : nodes) {
    ret += node->to_string() + "\n";
  }
  return ret;
}
#ifdef DISABLE_UNIT
TEST(parserTest, DeclTest1) {
  EXPECT_EQ(parse_text("let a=2,b=3,c=[1,2,3];"), "(vardecl (a 2 0)(b 3 1)(c (array [1,2,3]) 2))");
  EXPECT_EQ(parse_text("let b,c={a:123,b:[1,2,3]}"), "(vardecl (b 0)(c (object {a:123,b:(array [1,2,3])}) 1))");
  auto res = parse_blocks("let b,c={a:123,b:[1,2,3]};b=b+1;c=c+1;b=c=5+1;");
  EXPECT_NE(res.find("(ASSIGN b(0) (PLUS b(0) 1))"), std::string::npos);
  EXPECT_NE(res.find("(ASSIGN c(1) (PLUS c(1) 1))"), std::string::npos);
  EXPECT_NE(res.find("(ASSIGN b(0) (ASSIGN c(1) (PLUS 5 1)))"), std::string::npos);
  bool ok = false;
  try {
    auto res = parse_blocks("let b,c={a:123,b:[1,2,3]};\nb=b+1;\nc=c+1;\nb+1=5+1;");
  } catch (std::exception &e) {
    ok = true;
    EXPECT_EQ(std::string{e.what()},
              "syntax error: unknown file!:4:4:assign operation expect left value in the left side! ");
  }
  EXPECT_TRUE(ok);
}
#endif