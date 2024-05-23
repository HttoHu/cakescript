#include <parser/parser.h>
namespace cake {
AstNodePtr create_bin_op(AstNodePtr l, TokenKind op, AstNodePtr r);
AstNodePtr create_assign_op(AstNodePtr l, AstNodePtr r);

template <TokenKind BIN_OP> class BinOp : public AstNode {
public:
  BinOp(AstNodePtr _left, AstNodePtr _right) : left(std::move(_left)), right(std::move(_right)) {}
  bool left_value() const override { return false; }
  TmpObjectPtr eval() override { return TmpObjectPtr(eval_with_create(), true); }
  ObjectBase *eval_with_create() override {
    auto lval = left->eval();
    auto rval = right->eval();

#define BIN_OP_MP(TAG, OP)                                                                                             \
  if constexpr (BIN_OP == TAG)                                                                                         \
    return lval->OP(rval.get());

#define INT_BIN_OP(TAG, OP)                                                                                            \
  if constexpr (BIN_OP == TAG) {                                                                                       \
    auto l = dynamic_cast<IntegerObject *>(lval.get()), r = dynamic_cast<IntegerObject *>(rval.get());                 \
    if (!l || !r)                                                                                                      \
      cake_runtime_error("bitwise op: expect integer operand!");                                                       \
    return new IntegerObject(l->get_int() OP r->get_int());                                                            \
  }

    BIN_OP_MP(PLUS, add)
    BIN_OP_MP(MUL, mul)
    BIN_OP_MP(MINUS, sub)
    BIN_OP_MP(DIV, div)
    BIN_OP_MP(EQ, eq)
    BIN_OP_MP(NE, ne)
    BIN_OP_MP(GE, ge)
    BIN_OP_MP(GT, gt)
    BIN_OP_MP(LE, le)
    BIN_OP_MP(LT, lt)
    INT_BIN_OP(LSH, <<)
    INT_BIN_OP(MOD, %)
    INT_BIN_OP(RSH, >>)
    INT_BIN_OP(BIT_AND, &)
    INT_BIN_OP(BIT_OR, |)
    INT_BIN_OP(BIT_XOR, ^)
    INT_BIN_OP(AND, &&)
    INT_BIN_OP(OR, ||)
#undef BIN_OP_MP
#undef INT_BIN_OP
  }
  std::string to_string() const override {
    return "(" + Token::token_kind_str(BIN_OP) + " " + left->to_string() + " " + right->to_string() + ")";
  }
  ~BinOp() {}
private:
  AstNodePtr left, right;
};

template <TokenKind ASSIGN_OP> class AssignOp : public AstNode {
public:
  AssignOp(AstNodePtr _left,  AstNodePtr _right)
      : left(std::move(_left)), right(std::move(_right)) {}
  bool left_value() const override { return true; }
  TmpObjectPtr eval() override {
    auto left_val = left->get_left_val();
    if constexpr (ASSIGN_OP == ASSIGN) {
      // the object may be reassign an differnt type object, so we need delete old object
      TmpObjectPtr t(*left_val, true);
      // debug for a long time: right may use left_val, we can't delete left_val before right->eval_with_create()
      *left_val = right->eval_with_create();
      return *left_val;
    } else if constexpr (ASSIGN_OP == SADD) {
      (*left_val)->sadd(right->eval().get());
      return *left_val;
    } else if constexpr (ASSIGN_OP == SADD) {
      (*left_val)->sadd(right->eval().get());
      return *left_val;
    } else if constexpr (ASSIGN_OP == SSUB) {
      (*left_val)->ssub(right->eval().get());
      return *left_val;
    } else if constexpr (ASSIGN_OP == SMUL) {
      (*left_val)->smul(right->eval().get());
      return *left_val;
    } else if constexpr (ASSIGN_OP == SDIV) {
      (*left_val)->sdiv(right->eval().get());
      return *left_val;
    } else {
      abort();
    }
  }
  std::string to_string() const override{
    return "(" + Token::token_kind_str(ASSIGN_OP) + " " + left->to_string() + " " + right->to_string() + ")";
  }

private:
  AstNodePtr left, right;
};

} // namespace cake
