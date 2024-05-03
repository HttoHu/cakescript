#include <iostream>
#include <lexer.h>
#include <object.h>
#include <variant>

using namespace std;

int main() {
  cake::Scanner scanner("123_/* ddfd */if+3");
  std::cout << scanner.next_token().text << std::endl;
  std::cout << scanner.next_token().text << std::endl;
  std::cout << scanner.next_token().text << std::endl;
  return 0;
}