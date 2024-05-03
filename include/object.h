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
  virtual std::string to_string() const;

private:
};
class NumberObject : public ObjectBase {
public:
  NumberObject(int64_t val) : data(val) {}
  NumberObject(double val) : data(val) {}
  NumberObject() : data(std::monostate{}) {}
  
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

private:
  // if the state is monostate, the value is NaN
  variant<std::monostate, int64_t, double> data;
};
class StringObject : public ObjectBase {};

class ArrayObject {
public:
private:
  vector<ObjectBase *> objects;
};

} // namespace cake