#pragma once
#include <map>
#include <parser/ast_node.h>
#include <runtime/object.h>

#include <fmt/format.h>

namespace cake {
using std::map;
class BinOp : public AstNode {
public:
  BinOp(AstNodePtr _left, TokenKind _op, AstNodePtr _right)
      : left(std::move(_left)), right(std::move(_right)), op(_op) {}
  bool left_value() const override { return op == TokenKind::ASSIGN; }
  ObjectBase *eval() override;
  std::string to_string() const override {
    return fmt::format("({} {} {})", Token::token_kind_str(op), left->to_string(), right->to_string());
  }
  ~BinOp() {}

private:
  AstNodePtr left, right;
  TokenKind op;
};

class AssignOp : public AstNode {
public:
  AssignOp(AstNodePtr _left, TokenKind _op, AstNodePtr _right)
      : left(std::move(_left)), op(_op), right(std::move(_right)) {}
  bool left_value() const override { return true; }
  bool need_delete_eval_object() const override { return right->need_delete_eval_object(); }
  ObjectBase *eval() override;
  ObjectBase *eval_with_create() override {
    auto ret = eval();
    if (need_delete_eval_object())
      return ret;
    return ret->clone();
  }
  std::string to_string() const override;

private:
  TokenKind op;
  AstNodePtr left, right;
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
  ObjectBase *eval_with_create() override { return result_tmp->clone(); }
  std::string to_string() const override { return result_tmp->to_string(); }
  ~Literal() {
    if (result_tmp)
      delete result_tmp;
  }

private:
  ObjectBase *result_tmp;
  std::string get_str_from_literal(const std::string &text);
};

class Variable : public AstNode {
public:
  Variable(Token _id, size_t _stac_pos) : id(_id), stac_pos(_stac_pos) {}
  std::string to_string() const override { return fmt::format("{}({})", id.text, stac_pos); }
  bool left_value() const override { return true; }
  ObjectBase *eval() override;
  ObjectBase *eval_with_create() override { return eval()->clone(); }
  ObjectBase **get_left_val() override;

private:
  Token id;
  size_t stac_pos;
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
    return ret + ")";
  }

private:
  std::vector<AstNodePtr> nodes;
};

} // namespace cake