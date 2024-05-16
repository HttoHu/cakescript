#pragma once
#include <memory>
#include <string>
#include <utils.h>
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
  virtual bool is_true() const { abort(); }

  virtual ObjectBase *add(ObjectBase *rhs) { abort(); }
  virtual ObjectBase *sub(ObjectBase *rhs) { abort(); }
  virtual ObjectBase *mul(ObjectBase *rhs) { abort(); }
  virtual ObjectBase *div(ObjectBase *rhs) { abort(); }
  virtual ObjectBase *eq(ObjectBase *rhs) { abort(); }
  virtual ObjectBase *ne(ObjectBase *rhs) { abort(); }
  virtual ObjectBase *le(ObjectBase *rhs) { abort(); }
  virtual ObjectBase *lt(ObjectBase *rhs) { abort(); }
  virtual ObjectBase *gt(ObjectBase *rhs) { abort(); }
  virtual ObjectBase *ge(ObjectBase *rhs) { abort(); }
  virtual ObjectBase *apply(std::vector<ObjectBase *> args) { abort(); }
  virtual ObjectBase *&visit(int idx) { unreachable(); }
  virtual ObjectBase *&visit(std::string_view idx) { unreachable(); }

private:
};
class NumberObject : public ObjectBase {
public:
  using ValType = variant<int64_t, double>;
  NumberObject(ValType val) : data(val) {}
  NumberObject(bool val) : data((int64_t)val) {}
  NumberObject(int64_t val) : data(val) {}
  NumberObject(double val) : data(val) {}
  NumberObject() : data((int64_t)0) {}
  static int64_t get_integer_strict(ObjectBase *obj) {
    if (auto integer = dynamic_cast<NumberObject *>(obj))
      return std::get<int64_t>(integer->data);
    cake_runtime_error("get integer from an object " + obj->to_string() + "failed!");
  }
  bool is_true() const override {
    if (!data.index())
      return std::get<int64_t>(data);
    else
      return std::get<double>(data);
  }

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
  ObjectBase *add(ObjectBase *rhs) override;
  ObjectBase *sub(ObjectBase *rhs) override;
  ObjectBase *mul(ObjectBase *rhs) override;
  ObjectBase *div(ObjectBase *rhs) override;
  ObjectBase *eq(ObjectBase *rhs) override;
  ObjectBase *ne(ObjectBase *rhs) override;
  ObjectBase *le(ObjectBase *rhs) override;
  ObjectBase *lt(ObjectBase *rhs) override;
  ObjectBase *gt(ObjectBase *rhs) override;
  ObjectBase *ge(ObjectBase *rhs) override;
  ObjectBase *clone() const override { return new NumberObject(data); }

private:
  ValType data;
};
class NullObject : public ObjectBase {
public:
  std::string to_string() const override { return "null"; }

private:
};
class StringObject : public ObjectBase {
public:
  StringObject(std::string _str) : str(std::move(_str)) {}
  std::string to_string() const override { return str; }
  ObjectBase *add(ObjectBase *rhs) override;
  ObjectBase *clone() const override { return new StringObject(str); }

  std::string to_raw_format() const;

private:
  std::string str;
};

class ArrayObject : public ObjectBase {
public:
  ArrayObject(vector<ObjectBase *> vals) {
    objects = new ArrayData;
    objects->useCnt = 1;
    objects->arr = std::move(vals);
  }

  ObjectBase *clone() const override { return new ArrayObject(objects); }
  ObjectBase *&visit(int idx) override {
    if (idx >= objects->arr.size())
      cake_runtime_error("array object range overflow!");
    return objects->arr[idx];
  }
  std::string to_string() const override;
  ~ArrayObject() {
    objects->useCnt--;
    if (!objects->useCnt) {
      for (auto item : objects->arr)
        delete item;
      delete objects;
    }
  }
  size_t get_array_length() const { return objects->arr.size(); }

private:
  struct ArrayData {
    vector<ObjectBase *> arr;
    int useCnt = 0;
  };
  ArrayObject(ArrayData *val) : objects(val) { objects->useCnt++; }
  ArrayData *objects;
};

} // namespace cake