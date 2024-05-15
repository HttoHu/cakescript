#include <runtime/object.h>
#include <utils.h>

namespace cake {
#define NUMBER_BINOP(FUN_NAME, OP)                                                                                     \
  ObjectBase *NumberObject::FUN_NAME(ObjectBase *rhs) {                                                                \
    if (auto rv = dynamic_cast<NumberObject *>(rhs)) {                                                                 \
      if ((data.index() | rv->data.index()) == 0b00)                                                                   \
        return new NumberObject(std::get<int64_t>(data) OP std::get<int64_t>(rv->data));                               \
      else if ((data.index() & rv->data.index()) == 0b01)                                                              \
        return new NumberObject(std::get<double>(data) OP std::get<double>(rv->data));                                 \
      else if (data.index() == 0)                                                                                      \
        return new NumberObject(std::get<int64_t>(data) OP std::get<double>(rv->data));                                \
      else                                                                                                             \
        return new NumberObject(std::get<double>(data) OP std::get<int64_t>(rv->data));                                \
    } else                                                                                                             \
      unreachable();                                                                                                   \
  }

NUMBER_BINOP(add, +)
NUMBER_BINOP(sub, -)
NUMBER_BINOP(mul, *)
NUMBER_BINOP(eq, ==)
NUMBER_BINOP(ne, !=)
NUMBER_BINOP(le, <=)
NUMBER_BINOP(ge, >=)
NUMBER_BINOP(gt, >)
NUMBER_BINOP(lt, <)

ObjectBase *NumberObject::div(ObjectBase *rhs) {
  if (auto rv = dynamic_cast<NumberObject *>(rhs)) {
    return new NumberObject(to_double() / rv->to_double());
  } else {
    unreachable();
  }
}
#undef NUMBER_BINOP

ObjectBase *StringObject::add(ObjectBase *rhs) {
  if (auto rhs_str = dynamic_cast<StringObject *>(rhs)) {
    return new StringObject(str + rhs_str->str);
  } else
    throw std::runtime_error("StringObject::add, expected a string object of the right operand!");
}
} // namespace cake