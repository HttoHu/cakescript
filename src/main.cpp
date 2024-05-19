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
  if (argc < 2) {
    std::cerr << "expected <filename>!\n";
    exit(1);
  }
  bool dump_ast = argc >= 3 && argv[2] == std::string{"--dump-ast"};
  try {
    ifstream ifs(argv[1]);
    if (!ifs) {
      std::cerr << "open file " << argv[1] << " failed!";
    }
    std::string text((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    cake::Scanner scanner(text);
    cake::Parser parser(std::move(scanner));
    Context::global_context()->set_global_stmts(parser.parse_global());
    if (dump_ast)
      Context::global_context()->dump_stmts();
    else
      Context::global_context()->run();
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    exit(1);
  }
  return 0;
}