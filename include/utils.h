#pragma once
#include <iostream>
#include <optional>
namespace cake {
[[noreturn]] inline void unreachable() { abort(); }
namespace utils {
// if convert  failed return nullopt
std::optional<std::string> conv_escape(std::string_view text);
} // namespace utils
} // namespace cake