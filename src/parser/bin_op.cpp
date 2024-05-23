#include <parser/bin_op.h>
namespace cake {

#define CREATE_BINOP(KIND)                                                                                             \
  case KIND:                                                                                                           \
    return std::make_unique<BinOp<KIND>>(std::move(l), std::move(r));
#define CREATE_ASSIGN_OP(KIND)                                                                                         \
  case KIND:                                                                                                           \
    return std::make_unique<AssignOp<KIND>>(std::move(l), std::move(r));

AstNodePtr create_bin_op(AstNodePtr l, TokenKind op, AstNodePtr r) {
  using enum TokenKind;
  switch (op) {
    CREATE_BINOP(PLUS)
    CREATE_BINOP(MINUS)
    CREATE_BINOP(MUL)
    CREATE_BINOP(DIV)
    CREATE_BINOP(EQ)
    CREATE_BINOP(NE)
    CREATE_BINOP(GE)
    CREATE_BINOP(GT)
    CREATE_BINOP(LE)
    CREATE_BINOP(LT)
    CREATE_BINOP(LSH)
    CREATE_BINOP(RSH)
    CREATE_BINOP(MOD)
    CREATE_BINOP(BIT_AND)
    CREATE_BINOP(BIT_OR)
    CREATE_BINOP(BIT_XOR)
    CREATE_BINOP(AND)
    CREATE_BINOP(OR)
    CREATE_ASSIGN_OP(ASSIGN)
    CREATE_ASSIGN_OP(SADD)
    CREATE_ASSIGN_OP(SSUB)
    CREATE_ASSIGN_OP(SMUL)
    CREATE_ASSIGN_OP(SDIV)
  default:
    cake_runtime_error("bianry op: unsupported operation! " + Token::token_kind_str(op));
  }
}

#undef CREATE_BINOP
} // namespace cake