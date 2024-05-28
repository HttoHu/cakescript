#include <cstring>
#include <nrt/object.h>
namespace cake::nrt {
const int HASH_BASE = 257; 
uint32_t hash_base_pow[HASH_BASE_POW_LEN];

void init_hash_base_pow() {
  hash_base_pow[0] = 1;
  for (int i = 1; i < HASH_BASE_POW_LEN; i++) {
    hash_base_pow[i] = hash_base_pow[i - 1] * HASH_BASE;
  }
}

uint32_t JSString::hash_bytes(uint8_t buf[], uint32_t len) {
  uint32_t ret = 0;
  for (uint32_t i = 0; i < len; i++)
    ret = ret * (uint32_t)HASH_BASE + (uint32_t)buf[i];
  return ret;
}
JSString *JSString::create(std::string_view str) {
  JSString *ret = (JSString *)malloc(sizeof(JSString) + str.size());
  ret->len = str.size();
  ret->is_wide_char = false;
  ret->ref_cnt = 0;
  for (size_t i = 0; i < str.size(); i++) {
    ret->str8[i] = str[i];
  }
  ret->hash = hash_bytes(ret->str8, ret->len);
  return ret;
}
void JSString::free_string(JSString *str) { free(str); }
JSString *JSString::concat(JSString *l, JSString *r) {
  JSString *ret = (JSString *)malloc(sizeof(JSString) + l->len + r->len);
  ret->len = l->len + r->len;
  memcpy(ret->str8, l->str8, l->len);
  memcpy(ret->str8 + l->len, r->str8, r->len);
  if (r->len < HASH_BASE_POW_LEN)
    ret->hash = l->hash * hash_base_pow[r->len] + r->hash;
  else
    ret->hash = hash_bytes(ret->str8, ret->len);
  return ret;
}

std::string JSString::to_cxx_str(JSString *str) {
  std::string ret;
  ret.reserve(str->len);
  for (int i = 0; i < str->len; i++) {
    ret.push_back(str->str8[i]);
  }
  return ret;
}
} // namespace cake::nrt