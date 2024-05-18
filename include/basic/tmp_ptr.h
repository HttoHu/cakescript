#pragma once

namespace cake::basic {
template <typename T> class TmpPtr {
public:
  TmpPtr(T *dat, bool _need_to_relase = false) : data(dat), need_to_relase(_need_to_relase) {}
  // disable copy
  TmpPtr(const TmpPtr &rhs) = delete;

  TmpPtr(TmpPtr &&rhs) : data(rhs.data), need_to_relase(rhs.need_to_relase) {
    rhs.need_to_relase = false;
    rhs.data = nullptr;
  }
  TmpPtr &operator=(const TmpPtr &rhs) = delete;
  TmpPtr &operator=(TmpPtr &&rhs) {
    if (need_to_relase)
      delete data;
    need_to_relase = rhs.need_to_relase;
    data = rhs.data;
    rhs.data = nullptr, rhs.need_to_relase = false;
  }
  const T *operator->() const { return data; }
  T *operator->() { return data; }

  const T *get() const { return data; }
  T *get() { return data; }

  ~TmpPtr() {
    if (need_to_relase)
      delete data;
  }

private:
  T *data;
  bool need_to_relase = true;
};

} // namespace cake::basic