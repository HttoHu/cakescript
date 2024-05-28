#include <gtest/gtest.h>
#include <nrt/object.h>
#ifndef DISABLE_UNIT
TEST(NRT_TEST, StringTest) {
  using namespace cake::nrt;
  JSString *str = JSString::create("hello world!");
  EXPECT_EQ(str->len, 12);
  EXPECT_EQ(JSString::to_cxx_str(str), "hello world!");

  cake::nrt::init_hash_base_pow();
  JSString *hello = JSString::create("hello ");
  JSString *world = JSString::create("world!");
  JSString *hello_world = JSString::concat(hello, world);

  EXPECT_EQ(str->hash, hello_world->hash);
  EXPECT_EQ(JSString::to_cxx_str(hello_world), "hello world!");
  JSString::free_string(str);
  JSString::free_string(hello);
  JSString::free_string(world);
  JSString::free_string(hello_world);
}
#endif