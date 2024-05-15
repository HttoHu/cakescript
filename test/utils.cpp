#include <filesystem>
#include <fstream>
#include <string>
#include <utils.h>
namespace utils {
std::string read_file(const std::string rev_path) {
  using namespace std::filesystem;
  auto abs_path = canonical(path(__FILE__).parent_path().c_str() + std::string{"/"} + rev_path);

  std::ifstream t(abs_path);
  std::string text((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
  return text;
}
} // namespace utils