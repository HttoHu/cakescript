#pragma once
#include <object.h>
#include <parser/ast_node.h>
#include <fmt/format.h>

namespace cake {
class BinOp : public AstNode {
public:
  BinOp(AstNodePtr _left, TokenKind _op, AstNodePtr _right)
      : left(std::move(_left)), right(std::move(_right)), op(_op) {
    result_tmp = new NumberObject();
  }
  ObjectBase *eval() override;
  std::string to_string() const override {
    return fmt::format("({} {} {})", Token::token_kind_str(op), left->to_string(), right->to_string());
  }

private:
  AstNodePtr left, right;
  TokenKind op;
  ObjectBase *result_tmp;
};

class UnaryOp : public AstNode {
public:
  std::string to_string() const override {
    return fmt::format("({} {})", Token::token_kind_str(unary_op), expr->to_string());
  }
private:
  TokenKind unary_op;
  AstNodePtr expr;
  ObjectBase *result_tmp;
};

class Literal : public AstNode {
public:
  Literal(Token lit);
  ObjectBase *eval() override { return result_tmp; }
  std::string to_string() const override { return result_tmp->to_string(); }

private:
  ObjectBase *result_tmp;
};

} // namespace cake