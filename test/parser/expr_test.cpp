#include <gtest/gtest.h>
#include <iostream>
#include <parser/parser.h>

using namespace cake;

std::string parse_text(const std::string &str) {
  cake::Scanner scanner(str);
  cake::Parser parser(std::move(scanner));
  auto node = parser.parse_expr();
  return node->to_string();
}

TEST(parserTest, ExpressionTest1) { 
  EXPECT_EQ(parse_text("1+2*3"),"(PLUS 1 (MUL 2 3))"); 
  EXPECT_EQ(parse_text("(1+2)*3"),"(MUL (PLUS 1 2) 3)"); 
  EXPECT_EQ(parse_text("1+2+3+4+5"),"(PLUS (PLUS (PLUS (PLUS 1 2) 3) 4) 5)"); 
  EXPECT_EQ(parse_text("11+2*(3/5)"),"(PLUS 11 (MUL 2 (DIV 3 5)))"); 
  EXPECT_EQ(parse_text("11-(7+2)*(3/5)"),"(MINUS 11 (MUL (PLUS 7 2) (DIV 3 5)))"); 
}