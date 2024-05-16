#include <string>

namespace utils{
  // relative to ./test path 
  std::string read_file(const std::string& rev_path);
  std::string run_code(const std::string& text);
}