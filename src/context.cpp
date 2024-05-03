#include <context.h>

namespace cake {
Context::Context(){
  // all tokens have default file.
  source_file_list.push_back("unknown file!");
}

Context *Context::global_context() {
  static Context *ret;

  if (!ret) {
    ret = new Context();
    return ret;
  }
  return ret;
}
} // namespace cake