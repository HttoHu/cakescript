#pragma once
#include <map>
#include <object.h>
#include <parser/ast_node.h>

#include <fmt/format.h>

namespace cake {
using std::map;
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

// Number literal or string literal
class Literal : public AstNode {
public:
  Literal(Token lit);
  ObjectBase *eval() override { return result_tmp; }
  std::string to_string() const override { return result_tmp->to_string(); }

private:
  ObjectBase *result_tmp;
};

class ObjectNode : public AstNode {
public:
  ObjectNode(map<std::string, AstNodePtr> _object) : object(std::move(_object)) {}
  std::string to_string() const override {
    std::string ret = "(object {";
    for (auto &[name, val] : object) {
      ret += name + ":" + val->to_string() + ",";
    }
    if (object.size())
      ret.back() = '}';
    return ret + ")";
  }

private:
  map<std::string, AstNodePtr> object;
};

class ArrayNode : public AstNode {
public:
  ArrayNode(std::vector<AstNodePtr> &&_nodes) : nodes(std::move(_nodes)) {}
  std::string to_string() const override {
    std::string ret = "(array [";
    for (auto &node : nodes) {
      ret += node->to_string() + ",";
    }
    if (nodes.size())
      ret.back() = ']';
    return ret+")";
  }

private:
  std::vector<AstNodePtr> nodes;
};

} // namespace cake