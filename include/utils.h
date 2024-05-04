#pragma once
#include <iostream>

namespace cake{
  [[noreturn]] inline void unreachable(){
    abort();
  }
}