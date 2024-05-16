#include "../utils.h"
#include <gtest/gtest.h>
#include <parser/function.h>
#include <parser/parser.h>
#include <runtime/mem.h>
#include <context.h>
#include <parser/symbol.h>
#ifndef DISABLE_UNIT
TEST(runtimeTest, FunctionTest1) {
  using namespace cake;
  cake::Context::global_symtab()->new_block();
  auto text = ::utils::read_file("./data/function/case1.js");
  cake::Scanner scanner(text);
  cake::Parser parser(std::move(scanner));
  auto func = parser.parse_function_def();
  auto call = parser.parse_stmt();
  auto fib_func = parser.parse_function_def();
  auto call2 = parser.parse_stmt();

  EXPECT_EQ(call->to_string(), "(call test,1,100)");
  static_cast<cake::FunctionDef *>(func.get())->gen_func_object();
  EXPECT_EQ(call->eval()->to_string(), "4950");

  static_cast<cake::FunctionDef *>(fib_func.get())->gen_func_object();
  EXPECT_EQ(call2->to_string(), "(PLUS (call fib,8) (call test,1,10))");
  EXPECT_EQ(call2->eval()->to_string(), "66");
  cake::Context::global_context()->clear();
  Memory::gmem.clear();
  Memory::pc = 0;
}
#endif
