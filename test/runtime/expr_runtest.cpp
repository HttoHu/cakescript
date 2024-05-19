#include "../utils.h"
#include <gtest/gtest.h>
#include <parser/function.h>
#include <parser/parser.h>
#include <sstream>
#ifndef DISABLE_UNIT
#endif

TEST(runtimeTest, ExprTest1) {
  for (int i = 0; i < 2; i++) {
    auto text = utils::read_file("./data/basic/expr1.js");
    auto res = utils::run_code(text);
    std::stringstream ss(res);
    std::vector<std::string> expect_lines = {"4", "2", "33349", "520", "1", "1", "0", "1", "2914", "0", "6", "0", "10"};
    for (auto it : expect_lines) {
      std::string test;
      std::getline(ss, test);
      EXPECT_EQ(test, it);
    }
  }
}
