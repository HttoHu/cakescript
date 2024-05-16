#pragma once
#include <iostream>
#include <optional>
namespace cake {
[[noreturn]] inline void unreachable() {
  std::cerr << "runtime error!";
  exit(1);
}
[[noreturn]] inline void cake_runtime_error(std::string_view msg) {
  std::cout << msg << std::endl;
  exit(1);
}
namespace utils {
// if convert  failed return nullopt
std::optional<std::string> conv_escape(std::string_view text);
} // namespace utils
} // namespace cake