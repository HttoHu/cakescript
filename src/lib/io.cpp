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
void import_console(Context *con) {
  std::vector<std::pair<std::string, ObjectBase *>> tab;
  tab.push_back({"log", new InternalFunction(console_log)});
  StructObject *console_obj = new StructObject(std::move(tab));

  int pos = con->global_symtab()->cfunc_vcnt();
  auto init_stmt = std::make_unique<InternalObjCreate>(console_obj, pos);
  con->global_symtab()->add_global_symbol("console", new VarSymbol(pos, "console"));
  con->add_init_expr(std::move(init_stmt));
}
} // namespace cake::lib