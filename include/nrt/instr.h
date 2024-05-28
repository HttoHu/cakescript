#pragma once
#include <cinttypes>
/*
  a three-addr risc-like IR
*/
namespace cake::nrt {
// AstNode -> SSA Instr directly ?
// clang-format off 
enum class OpCode : uint16_t { 
  PLUS, MINUS, MUL, DIV, INC, DEC, 
  VISIT_PROP, 
  VISIT_PROP_BY_IDX,
  PHI,
};
// clang-format on 
using operand_index_type = uint16_t;

// to test if the instruction is a pseudo instruction, pseudo instruction is intermediate of optimization and canZ not
// run.
bool isPseudoOp(OpCode op_code);
struct Instr {
  OpCode op_code;
  operand_index_type target;
  operand_index_type left;
  operand_index_type right;
  union {
    void *extra_info;
    uint64_t abs_pos;
    operand_index_type operand3;
  };
};

} // namespace cake::codegen