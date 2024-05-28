#pragma once
#include <cinttypes>
#include <string>
#define HASH_BASE_POW_LEN 4096
namespace cake::nrt {

// clang-format off
enum ObjecType { 
  OBJ_FLOAT, OBJ_INTEGER, 
  OBJ_ARR, OBJ_STRING, OBJ_COMMON, 
  OBJ_NULL, OBJ_UNDEF 
};
extern uint32_t hash_base_pow[HASH_BASE_POW_LEN];
void init_hash_base_pow();
// clang-format on
struct JSValue {
  uint64_t tag;
  union {
    int64_t int_val;
    double float_val;
    void *ptr;
  };
};
// string don't have children, we use reference counter to manage memory
struct JSString {
  int32_t ref_cnt = 0;
  uint32_t len : 31;
  bool is_wide_char : 1;
  uint32_t hash;
  union {
    uint8_t str8[0];
    uint16_t str16[0];
  };
  static uint32_t hash_bytes(uint8_t buf[], uint32_t len);
  static JSString *create(std::string_view str);
  static void free_string(JSString *str);
  static JSString *concat(JSString *l, JSString *r);
  static std::string to_cxx_str(JSString *str);
};

struct JSArray {
  uint32_t len : 31 = 0;
  // to check if all elements have the same type
  bool type_consistant : 1;
  uint32_t capacity = 0;
  JSValue *data = nullptr;
};

struct JSObjectProperty {
  JSString *key;
  JSValue *val;
};

struct JSObject {
  JSObject *proto = nullptr;
  JSObjectProperty *property_list = nullptr;
};
} // namespace cake::nrt