#include <gtest/gtest.h>
#include <iostream>
#include <parser/parser.h>

using namespace cake;

static std::string parse_text(const std::string &str) {
  cake::Scanner scanner(str);
  cake::Parser parser(std::move(scanner));
  auto node = parser.parse_expr();
  return node->to_string();
}

static std::string eval_text(const std::string &str) {
  cake::Scanner scanner(str);
  cake::Parser parser(std::move(scanner));
  auto node = parser.parse_expr();
  std::string ret;
  auto obj = node->eval_with_create();
  ret = obj->to_string();
  delete obj;
  return ret;
}

#ifndef DISABLE_UNIT

TEST(parserTest, ExpressionTest1) {
  EXPECT_EQ(parse_text("1+2*3"), "(PLUS 1 (MUL 2 3))");
  EXPECT_EQ(parse_text("(1+2)*3"), "(MUL (PLUS 1 2) 3)");
  EXPECT_EQ(parse_text("1+2+3+4+5"), "(PLUS (PLUS (PLUS (PLUS 1 2) 3) 4) 5)");
  EXPECT_EQ(parse_text("11+2*(3/5)"), "(PLUS 11 (MUL 2 (DIV 3 5)))");
  EXPECT_EQ(parse_text("11-(7+2)*(3/5)"), "(MINUS 11 (MUL (PLUS 7 2) (DIV 3 5)))");
  EXPECT_EQ(eval_text("1+1"), "2");
  EXPECT_EQ(eval_text("2*3"), "6");
  EXPECT_EQ(eval_text("1-(1-5)"), "5");
  EXPECT_EQ(eval_text("2+3*5-3"), "14");
}

TEST(parserTest, ExpressionTest2) {
  EXPECT_EQ(parse_text("[1,2,3,4]"), "(array [1,2,3,4])");
  EXPECT_EQ(parse_text("{a:1,b:2,c:[1,2,3],d:{a:1,b:1}}"), "(object {a:1,b:2,c:(array [1,2,3]),d:(object {a:1,b:1})})");
}
#endif
