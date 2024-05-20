#include <map>
#include <runtime/intern_function.h>
#include <runtime/object.h>
#include <utils.h>

namespace cake {
UndefinedObject *UndefinedObject::global_undefined_obj = new UndefinedObject;
NullObject *NullObject::global_null_obj = new NullObject;

#define NUMBER_BINOP(FUN_NAME, OP)                                                                                     \
  ObjectBase *IntegerObject::FUN_NAME(ObjectBase *rhs) {                                                               \
    if (auto rv = dynamic_cast<IntegerObject *>(rhs)) {                                                                \
      return new IntegerObject(data OP rv->data);                                                                      \
    } else                                                                                                             \
      unreachable();                                                                                                   \
  }

NUMBER_BINOP(add, +)
NUMBER_BINOP(sub, -)
NUMBER_BINOP(mul, *)
NUMBER_BINOP(div, /)
NUMBER_BINOP(eq, ==)
NUMBER_BINOP(ne, !=)
NUMBER_BINOP(le, <=)
NUMBER_BINOP(ge, >=)
NUMBER_BINOP(gt, >)
NUMBER_BINOP(lt, <)
#undef NUMBER_BINOP

#define NUMBER_SELFASSIGN_OP(FUN_NAME, OP)                                                                             \
  void IntegerObject::FUN_NAME(ObjectBase *rhs) {                                                                      \
    if (auto rv = dynamic_cast<IntegerObject *>(rhs)) {                                                                \
      data OP rv->data;                                                                                                \
    } else                                                                                                             \
      unreachable();                                                                                                   \
  }

NUMBER_SELFASSIGN_OP(sadd, +=)
NUMBER_SELFASSIGN_OP(ssub, -=)
NUMBER_SELFASSIGN_OP(smul, -=)
NUMBER_SELFASSIGN_OP(sdiv, -=)
#undef NUMBER_SELFASSIGN_OP

ObjectBase *StringObject::add(ObjectBase *rhs) {
  if (auto rhs_str = dynamic_cast<StringObject *>(rhs)) {
    return new StringObject(str + rhs_str->str);
  } else
    throw std::runtime_error("StringObject::add, expected a string object of the right operand!");
}
ObjectBase *StringObject::visitVal(int idx) { return new StringObject(str.substr(0, 1)); }

ObjectBase *StringObject::visitVal(const std::string &idx) {
  if (idx == "length")
    return new IntegerObject((int64_t)str.size());
  static std::map<std::string, InternalFunction *> tab = {
      {"set", new InternalFunction(inter_funcs::_string_set)},
      {"trim", new InternalFunction(inter_funcs::_string_trim)},
      {"indexOf", new InternalFunction(inter_funcs::_string_index_of)}};
  auto res = tab.find(idx);
  if (res == tab.end())
    return new UndefinedObject;
  return res->second->clone();
}
void StringObject::sadd(ObjectBase *rhs) { str += StringObject::get_str_from_obj(rhs); }
std::string StringObject::to_raw_format() const {
  std::string ret = "\"";
  for (auto ch : str) {
    switch (ch) {
    case '\n':
      ret += "\\n";
      break;
    case '\t':
      ret += "\\t";
      break;
    case '\"':
      ret += "\\\"";
      break;
    default:
      ret += ch;
    }
  }
  return ret + "\"";
}

ObjectBase *ArrayObject::visitVal(const string &idx) {
  if (idx == "length")
    return new IntegerObject((int64_t)objects->arr.size());

  static std::map<std::string, InternalFunction *> tab = {{"push", new InternalFunction(inter_funcs::_array_push)},
                                                          {"pop", new InternalFunction(inter_funcs::_array_pop)},
                                                          {"resize", new InternalFunction(inter_funcs::_array_resize)}};
  auto res = tab.find(idx);
  if (res == tab.end())
    return new UndefinedObject;
  return res->second->clone();
}
std::string ArrayObject::to_string() const {
  std::string ret = "[ ";
  bool first = true;
  for (auto &mem : objects->arr) {
    if (!first)
      ret += ", ";
    first = false;
    if (auto str = dynamic_cast<StringObject *>(mem))
      ret += str->to_raw_format();
    else
      ret += mem->to_string();
  }
  return ret + " ]";
}
void ArrayObject::resize(size_t newSz) {
  int dif = (int)newSz - (int)objects->arr.size();
  if (dif > 0)
    for (int i = 0; i < dif; i++)
      objects->arr.push_back(new NullObject);
  else {
    for (int i = 0; i < -dif; i++) {
      delete objects->arr.back();
      objects->arr.pop_back();
    }
  }
}
} // namespace cake