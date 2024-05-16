#include "../utils.h"
#include <gtest/gtest.h>
#include <parser/function.h>
#include <parser/parser.h>
#ifndef DISABLE_UNIT
TEST(runtimeTest, ArrayTest1) { 
  auto text = utils::read_file("./data/basic/array1.js"); 
  auto res = utils::run_code(text);
  std::cout<<res<<std::endl;
}
#endif
