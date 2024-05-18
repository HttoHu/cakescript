#pragma once
#include <algorithm>
#include <map>
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
  // the return value must be cloned.
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

  virtual void sadd(ObjectBase *rhs) { cake_runtime_error("unsupport operation +="); }
  virtual void ssub(ObjectBase *rhs) { cake_runtime_error("unsupport operation -="); }
  virtual void sdiv(ObjectBase *rhs) { cake_runtime_error("unsupport operation /="); }
  virtual void smul(ObjectBase *rhs) { cake_runtime_error("unsupport operation *="); }
  virtual void inc() { cake_runtime_error("unsupport operation ++"); }
  virtual void dec() { cake_runtime_error("unsupport operation --"); }

  // the return value should be cloned
  virtual ObjectBase *apply(std::vector<ObjectBase *> args) { abort(); }
  virtual ObjectBase **visit(int idx) { unreachable(); }
  // must cloned object
  virtual ObjectBase *visitVal(int idx) { unreachable(); }
  virtual ObjectBase **visit(const string &idx) { unreachable(); }
  // return cloned object
  virtual ObjectBase *visitVal(const string &idx) { unreachable(); }

private:
};

class IntegerObject : public ObjectBase {
public:
  using ValType = variant<int64_t, double>;
  IntegerObject(bool val) : data((int64_t)val) {}
  IntegerObject(int64_t val) : data(val) {}
  IntegerObject() : data((int64_t)0) {}
  static int64_t get_integer(ObjectBase *obj) {
    if (auto integer = dynamic_cast<IntegerObject *>(obj))
      return integer->data;
    cake_runtime_error("get integer from an object " + obj->to_string() + "failed!");
  }
  bool is_true() const override { return data; }
  int64_t get_int() const { return data; }
  std::string to_string() const override { return std::to_string(data); }
  void reset_val(int64_t val) { data = val; }
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
  ObjectBase *clone() const override { return new IntegerObject(data); }
  void sadd(ObjectBase *rhs) override;
  void ssub(ObjectBase *rhs) override;
  void sdiv(ObjectBase *rhs) override;
  void smul(ObjectBase *rhs) override;
  // void inc() override;
  // void dec() override;

private:
  int64_t data;
};
class NullObject : public ObjectBase {
public:
  std::string to_string() const override { return "null"; }
  ObjectBase *clone() const override { return new NullObject; }
  static NullObject *global_null_obj;

private:
};
class UndefinedObject : public ObjectBase {
public:
  std::string to_string() const override { return "undefined"; }
  ObjectBase *clone() const override { return new UndefinedObject; }
  static UndefinedObject *global_undefined_obj;

private:
};
class StringObject : public ObjectBase {
public:
  StringObject(std::string _str) : str(std::move(_str)) {}
  std::string to_string() const override { return str; }
  ObjectBase *add(ObjectBase *rhs) override;
  ObjectBase *clone() const override { return new StringObject(str); }

  std::string to_raw_format() const;

  std::string str;

private:
};

class ArrayObject : public ObjectBase {
public:
  ArrayObject(vector<ObjectBase *> vals) {
    objects = new ArrayData;
    objects->useCnt = 1;
    objects->arr = std::move(vals);
  }

  ObjectBase *clone() const override { return new ArrayObject(objects); }
  ObjectBase **visit(int idx) override {
    if (idx >= objects->arr.size())
      return nullptr;
    return &objects->arr[idx];
  }
  ObjectBase *visitVal(int idx) override {
    if (idx >= objects->arr.size())
      return new UndefinedObject;
    return objects->arr[idx]->clone();
  }
  ObjectBase *visitVal(const string &idx) override;
  std::string to_string() const override;
  ~ArrayObject() {
    objects->useCnt--;
    if (!objects->useCnt) {
      delete objects;
    }
  }
  size_t get_array_length() const { return objects->arr.size(); }

  // caller make sure obj is cloned
  void push_obj(ObjectBase *obj) { objects->arr.push_back(obj); }
  void pop() {
    delete objects->arr.back();
    objects->arr.pop_back();
  }

private:
  struct ArrayData {
    vector<ObjectBase *> arr;
    int useCnt = 0;
    ~ArrayData() {
      for (auto item : arr)
        delete item;
    }
  };
  ArrayObject(ArrayData *val) : objects(val) { objects->useCnt++; }
  ArrayData *objects;
};

class StructObject : public ObjectBase {
private:
  using StructTabTy = std::vector<std::pair<std::string, ObjectBase *>>;

  struct StructData {
    StructData(StructTabTy _tab) : tab(std::move(_tab)), useCnt(1) {}
    std::vector<std::pair<std::string, ObjectBase *>> tab;
    ObjectBase **addNewObject(const std::string &key, ObjectBase *val) {
      auto res = std::lower_bound(tab.begin(), tab.end(), std::pair<std::string, ObjectBase *>{key, nullptr},
                                  [](const auto &p1, const auto &p2) { return p1.first < p2.first; });
      if (res->first == key) {
        res->second = val;
        return &res->second;
      }
      tab.push_back({key, val});
      return &tab.back().second;
    }
    ObjectBase *&findObject(const std::string &key) {
      auto res = std::lower_bound(tab.begin(), tab.end(), std::pair<std::string, ObjectBase *>{key, nullptr},
                                  [](const auto &p1, const auto &p2) { return p1.first < p2.first; });
      if (res->first == key)
        return res->second;
      cake_runtime_error("undefined member " + key);
    }
    int useCnt;
    ~StructData() {
      for (auto &[k, v] : tab)
        delete v;
    }
  } * data;
  StructObject(StructData *_data) : data(_data) { data->useCnt++; }

public:
  StructObject(StructTabTy tab) { data = new StructData(std::move(tab)); }
  ~StructObject() {
    data->useCnt--;
    if (!data->useCnt)
      delete data;
  }
  StructObject *clone() const override { return new StructObject(data); }
  ObjectBase **visit(const string &idx) override {
    for (auto &item : data->tab) {
      if (item.first == idx)
        return &item.second;
    }
    data->tab.push_back({idx, nullptr});
    return &data->tab.back().second;
  }
  ObjectBase *visitVal(const std::string &idx) override {
    for (auto &item : data->tab) {
      if (item.first == idx)
        return item.second->clone();
    }
    return new UndefinedObject;
  }
  std::string to_string() const override {
    std::string ret = "{ ";
    bool first = true;
    for (auto &[key, val] : data->tab) {
      if (!first)
        ret += ", ";
      first = false;
      ret += key + ": " + val->to_string();
    }
    ret += "}";
    return ret;
  }
};
} // namespace cake