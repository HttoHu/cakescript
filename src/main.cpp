#include <context.h>
#include <fstream>
#include <iostream>
#include <lexer.h>
#include <parser.h>
#include <runtime/mem.h>
#include <runtime/object.h>
#include <variant>
using namespace std;
using namespace cake;

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "expected <filename>!\n";
    exit(1);
  }
  try {
    ifstream ifs(argv[1]);
    if (!ifs) {
      std::cerr << "open file " << argv[1] << " failed!";
    }
    std::string text((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    cake::Scanner scanner(text);
    cake::Parser parser(std::move(scanner));
    auto nodes = parser.parse_global();
    Memory::gmem.new_func(cake::Context::global_context()->cblk_vcnt());
    for (Memory::pc = 0; Memory::pc < nodes.size(); Memory::pc++) {
      nodes[Memory::pc]->eval();
    }
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    exit(1);
  }
  return 0;
}