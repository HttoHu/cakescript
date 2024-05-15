#include <context.h>
#include <gtest/gtest.h>
#include <lexer.h>

using namespace cake;
#ifndef DISABLE_UNIT

TEST(lexerTest, simpleCase) {
  Scanner scanner("145/**/+ cat\nbbk \"good\"");
#define TEST_TOKEN(idx, KIND, TEXT, LINE, COL)                                                                         \
  EXPECT_EQ(scanner.peek(idx).kind, TokenKind::KIND);                                                                  \
  EXPECT_EQ(scanner.peek(idx).text, TEXT);                                                                             \
  EXPECT_EQ(scanner.peek(idx).line, LINE);                                                                             \
  EXPECT_EQ(scanner.peek(idx).col, COL);

  TEST_TOKEN(0, INTEGER, "145", 1, 1)
  TEST_TOKEN(1, PLUS, "+", 1, 8)
  TEST_TOKEN(2, IDENTIFIER, "cat", 1, 10)
  scanner.next_token();
  scanner.next_token();
  scanner.next_token();
  TEST_TOKEN(0, IDENTIFIER, "bbk", 2, 1)
  TEST_TOKEN(1, STRING, "\"good\"", 2, 5)

  TEST_TOKEN(100, NIL, "", 2, 10)

#undef TEST_TOKEN
}
#endif