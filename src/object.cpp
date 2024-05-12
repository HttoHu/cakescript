#include <object.h>
#include <utils.h>

namespace cake {
#define NUMBER_BINOP(OP)                                                                                               \
  auto rs = dynamic_cast<NumberObject *>(result);                                                                      \
  if (!rs)                                                                                                             \
    rs = new NumberObject();                                                                                           \
  if ((data.index() | rv->data.index()) == 0b00)                                                                       \
    rs->reset_val(std::get<int64_t>(data) OP std::get<int64_t>(rv->data));                                             \
  else if ((data.index() & rv->data.index()) == 0b01)                                                                  \
    rs->reset_val(std::get<double>(data) OP std::get<double>(rv->data));                                               \
  else if (data.index() == 0)                                                                                          \
    rs->reset_val(std::get<int64_t>(data) OP std::get<double>(rv->data));                                              \
  else                                                                                                                 \
    rs->reset_val(std::get<double>(data) OP std::get<int64_t>(rv->data));                                              \
  return rs

ObjectBase *NumberObject::add(ObjectBase *rhs, ObjectBase *result) {
  if (auto rv = dynamic_cast<NumberObject *>(rhs)) {
    NUMBER_BINOP(+);
  } else {
    unreachable();
  }
}
ObjectBase *NumberObject::sub(ObjectBase *rhs, ObjectBase *result) {
  if (auto rv = dynamic_cast<NumberObject *>(rhs)) {
    NUMBER_BINOP(-);
  } else {
    unreachable();
  }
}
ObjectBase *NumberObject::mul(ObjectBase *rhs, ObjectBase *result) {
  if (auto rv = dynamic_cast<NumberObject *>(rhs)) {
    NUMBER_BINOP(*);

  } else {
    unreachable();
  }
}
ObjectBase *NumberObject::div(ObjectBase *rhs, ObjectBase *result) {
  if (auto rv = dynamic_cast<NumberObject *>(rhs)) {
    auto rs = dynamic_cast<NumberObject *>(result);
    if (!rs)
      rs = new NumberObject();
    rs->reset_val(to_double() / to_double());
    return rs;
  } else {
    unreachable();
  }
}
ObjectBase *NumberObject::eq(ObjectBase *rhs, ObjectBase *result) {
  if (auto rv = dynamic_cast<NumberObject *>(rhs)) {
    NUMBER_BINOP(==);
  } else {
    unreachable();
  }
}

ObjectBase *NumberObject::ne(ObjectBase *rhs, ObjectBase *result) {
  if (auto rv = dynamic_cast<NumberObject *>(rhs)) {
    NUMBER_BINOP(!=);
  } else {
    unreachable();
  }
}
#undef NUMBER_BINOP
} // namespace cake