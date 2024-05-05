#include <context.h>
#include <gtest/gtest.h>
#include <parser/decl.h>
#ifndef DISABLE_UNIT
static std::string parse_text(const std::string &str) {
  cake::Scanner scanner(str);
  cake::Parser parser(std::move(scanner));
  auto node = parser.parse_stmt();
  cake::Context::global_context()->clear();
  return node->to_string();
}
TEST(parserTest, DeclTest1) {
  EXPECT_EQ(parse_text("let a=2,b=3,c=[1,2,3];"), "(vardecl (a 2 0)(b 3 1)(c (array [1,2,3]) 2))");
  EXPECT_EQ(parse_text("let b,c={a:123,b:[1,2,3]}"),"(vardecl (b 0)(c (object {a:123,b:(array [1,2,3])}) 1))");
}
#endif