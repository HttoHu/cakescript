#include "../utils.h"
#include <gtest/gtest.h>
#include <parser/function.h>
#include <parser/parser.h>
#include <sstream>
#ifndef DISABLE_UNIT
TEST(runtimeTest, ArrayTest1) {
  auto text = utils::read_file("./data/basic/array1.js");
  auto res = utils::run_code(text);
  std::stringstream ss(res);
  std::string test;
  std::getline(ss, test);
  EXPECT_EQ(test, "-1");
  std::getline(ss, test);
  EXPECT_EQ(test, "2");
  std::getline(ss, test);
  EXPECT_EQ(test, "3");
  std::getline(ss, test);
  EXPECT_EQ(test, "damn it ");
  std::getline(ss, test);
  EXPECT_EQ(test, "4");
  std::getline(ss, test);
  EXPECT_EQ(test, "---[ 3, \"hell\" ]");
}
#endif
