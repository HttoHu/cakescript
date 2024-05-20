#include <context.h>
#include <lib/io.h>
#include <parser/expr.h>
#include <parser/symbol.h>
#include <runtime/intern_function.h>
#include <runtime/object.h>
namespace cake::lib {
ObjectBase *console_log(std::vector<ObjectBase *> args) {
  for (int i = 0; i < args.size() - 1; i++)
    std::cout << args[i]->to_string();
  std::cout << std::endl;
  return new NullObject();
}
ObjectBase *console_print(std::vector<ObjectBase *> args) {
  for (int i = 0; i < args.size() - 1; i++)
    std::cout << args[i]->to_string();
  return new NullObject();
}
ObjectBase *console_read_int(std::vector<ObjectBase *> args) {
  int64_t res;
  std::cin >> res;
  return new IntegerObject(res);
}
ObjectBase *console_read_string(std::vector<ObjectBase *> args) {
  std::string str;
  std::cin >> str;
  return new StringObject(str);
}
ObjectBase *console_read_line(std::vector<ObjectBase *> args) {
  std::string line;
  std::getline(std::cin, line);
  return new StringObject(line);
}
void import_console(Context *con) {
  std::vector<std::pair<std::string, ObjectBase *>> tab;
  tab.push_back({"log", new InternalFunction(console_log)});
  tab.push_back({"print", new InternalFunction(console_print)});
  tab.push_back({"readInt", new InternalFunction(console_read_int)});
  tab.push_back({"readString", new InternalFunction(console_read_string)});
  tab.push_back({"readLine", new InternalFunction(console_read_line)});

  StructObject *console_obj = new StructObject(std::move(tab));

  int pos = con->global_symtab()->cfunc_vcnt();
  auto init_stmt = std::make_unique<InternalObjCreate>(console_obj, pos);
  con->global_symtab()->add_global_symbol("console", new VarSymbol(pos, "console"));
  con->add_init_expr(std::move(init_stmt));
}
} // namespace cake::lib