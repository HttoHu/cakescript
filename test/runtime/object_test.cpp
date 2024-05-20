#include "../utils.h"
#include <gtest/gtest.h>
#include <parser/function.h>
#include <parser/parser.h>
#include <sstream>
#ifndef DISABLE_UNIT
TEST(runtimeTest, ObjectTest2) {
  auto text = utils::read_file("./data/basic/object1.js");
  auto res = utils::run_code(text);
  std::stringstream ss(res);
  std::string test;
  std::getline(ss, test);
  EXPECT_EQ(test, "undefined");
  std::getline(ss, test);
  EXPECT_EQ(test, "4");
  std::getline(ss, test);
  EXPECT_EQ(test, "1");
  std::getline(ss, test);
  EXPECT_EQ(test, "6");
  std::getline(ss, test);
  EXPECT_EQ(test, "{ a: [ 1, 2, 3, 4, 5 ], b: 2}");
  std::getline(ss, test);
  EXPECT_EQ(test, "2");
  std::getline(ss, test);
  EXPECT_EQ(test, R"({ a: [ 1, 2, 3, 4, 5 ], b: { b: 4, e: 5, k: 3, "k": 4, "k1": 4, k3: 4}})");
}
#endif
