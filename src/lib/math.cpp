#include <context.h>
#include <parser/expr.h>
#include <parser/symbol.h>
#include <runtime/intern_function.h>
#include <runtime/object.h>
namespace cake::lib {
ObjectBase *math_abs(std::vector<ObjectBase *> args) {
  auto val = IntegerObject::get_integer(args[0]);
  if(val < 0)
    return new IntegerObject(-val);
  return new IntegerObject(val);
}
void import_Math(Context *con) {
  std::vector<std::pair<std::string, ObjectBase *>> tab;
  tab.push_back({"abs", new InternalFunction(math_abs)});

  StructObject *math_obj = new StructObject(std::move(tab));

  int pos = con->global_symtab()->cfunc_vcnt();
  auto init_stmt = std::make_unique<InternalObjCreate>(math_obj, pos);
  con->global_symtab()->add_global_symbol("Math", new VarSymbol(pos, "Math"));
  con->add_init_expr(std::move(init_stmt));
}
} // namespace cake::lib