#include <context.h>
#include <filesystem>
#include <fstream>
#include <parser/parser.h>
#include <parser/symbol.h>
#include <runtime/mem.h>
#include <string>
#include <utils.h>
namespace utils {
std::string read_file(const std::string &rev_path) {
  using namespace std::filesystem;
  auto abs_path = canonical(path(__FILE__).parent_path().c_str() + std::string{"/"} + rev_path);

  std::ifstream t(abs_path);
  std::string text((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
  return text;
}

std::string run_code(const std::string &text) {
  using namespace cake;
  cake::Scanner scanner(text);
  cake::Parser parser(std::move(scanner));
  std::stringstream ss;
  auto old_cout = std::cout.rdbuf();
  std::cout.rdbuf(ss.rdbuf());
  Context::global_context()->set_global_stmts(parser.parse_global());
  Context::global_context()->run();

  std::cout.rdbuf(old_cout);
  return ss.str();
}
} // namespace utils