#pragma once
#include <string>
#include <variant>
#include <vector>

namespace cake {
using std::string;
using std::variant;
using std::vector;

class ObjectBase {
public:
  virtual ~ObjectBase() {}
  virtual std::string to_string() const { return "undefined"; }
  virtual ObjectBase *clone() const { abort(); }
  // if the result type is matched return result, else return a new object of result
  virtual ObjectBase *add(ObjectBase *rhs, ObjectBase *result) { abort(); }
  // ditto
  virtual ObjectBase *sub(ObjectBase *rhs, ObjectBase *result) { abort(); }
  virtual ObjectBase *mul(ObjectBase *rhs, ObjectBase *result) { abort(); }
  virtual ObjectBase *div(ObjectBase *rhs, ObjectBase *result) { abort(); }

private:
};
class NumberObject : public ObjectBase {
public:
  using ValType = variant<int64_t, double>;
  NumberObject(ValType val) : data(val) {}

  NumberObject(int64_t val) : data(val) {}
  NumberObject(double val) : data(val) {}
  NumberObject() : data((int64_t)0) {}

  std::string to_string() const override {
    std::string ret;
    std::visit(
        [&](auto &&val) {
          using T = std::decay_t<decltype(val)>;
          if constexpr (std::is_same_v<T, int64_t>) {
            ret = std::to_string(val);
          } else if constexpr (std::is_same_v<T, double>)
            ret = std::to_string(val);
          else
            ret = "NaN";
        },
        data);
    return ret;
  }
  double to_double() const {
    if (!data.index())
      return std::get<int64_t>(data);
    return std::get<double>(data);
  }

  int64_t to_int() const {
    if (!data.index())
      return std::get<int64_t>(data);
    return std::get<double>(data);
  }

  void reset_val(ValType val) { data = val; }
  ObjectBase *add(ObjectBase *rhs, ObjectBase *result) override;
  ObjectBase *sub(ObjectBase *rhs, ObjectBase *result) override;
  ObjectBase *mul(ObjectBase *rhs, ObjectBase *result) override;
  ObjectBase *div(ObjectBase *rhs, ObjectBase *result) override;

  ObjectBase *clone() const override { return new NumberObject(data); }

private:
  ValType data;
};
class StringObject : public ObjectBase {
public:
private:
};

class ArrayObject {
public:
private:
  vector<ObjectBase *> objects;
};

} // namespace cake