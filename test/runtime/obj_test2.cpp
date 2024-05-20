#include "../utils.h"
#include <gtest/gtest.h>
#include <parser/function.h>
#include <parser/parser.h>
#include <sstream>
#ifndef DISABLE_UNIT
TEST(runtimeTest, ObjectTest1) {
  auto text = utils::read_file("./data/basic/object2.js");
  auto res = utils::run_code(text);
  std::stringstream ss(res);
  std::vector<std::string> expect_lines = {
      "15", "gell wolrdw", "6", "11", "gell wolrdwkkk", "[ null, null, null, null, null, null, null, null, null, 1 ]"};
  for (auto it : expect_lines) {
    std::string test;
    std::getline(ss, test);
    EXPECT_EQ(test, it);
  }
}
#endif
