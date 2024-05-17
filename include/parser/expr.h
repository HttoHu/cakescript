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
  ObjectBase *eval_with_create() override;
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
  ObjectBase *eval() override;
  ObjectBase *eval_with_create() override {
    return eval()->clone();
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

template <typename INDEX_TY> class VistorMember : public AstNode {
public:
  VistorMember(AstNodePtr _left, INDEX_TY _index) : left(std::move(_left)), index(std::move(_index)) {}
  std::string to_string() const override {
    if constexpr (std::is_same_v<INDEX_TY, int64_t>)
      return "(array_visit " + left->to_string() + " " + std::to_string(index) + ")";
    else if constexpr (std::is_same_v<INDEX_TY, std::string>)
      return "(array_visit " + left->to_string() + " " + index + ")";
    else
      return "(array_visit " + left->to_string() + " " + index->to_string() + ")";
  }
  ObjectBase *eval() override {
    auto left_val = left->eval_with_create();

    if constexpr (std::is_same_v<INDEX_TY, int64_t>)
      return left_val->visitVal(index);
    else if constexpr (std::is_same_v<INDEX_TY, std::string>)
      return left_val->visitVal(index);
    else {
      auto index_obj = index->eval_with_create();
      if (auto *int_index = dynamic_cast<NumberObject *>(index_obj))
        return left_val->visitVal(int_index->to_int());
      else if (auto *str_index = dynamic_cast<StringObject *>(index_obj))
        return left_val->visitVal(str_index->str);
      return new UndefinedObject;
    }
  }
  ObjectBase *eval_with_create() override { return eval(); }
  void eval_no_value() override { delete eval(); }

  bool left_value() const override { return true; }
  ObjectBase **get_left_val() override {
    auto left_val = left->eval_with_create();

    if constexpr (std::is_same_v<INDEX_TY, int64_t>)
      return left_val->visit(index);
    else if constexpr (std::is_same_v<INDEX_TY, std::string>)
      return left_val->visit(index);
    else {
      auto index_obj = index->eval_with_create();
      if (auto *int_index = dynamic_cast<NumberObject *>(index_obj))
        return left_val->visit(int_index->to_int());
      else if (auto *str_index = dynamic_cast<StringObject *>(index_obj))
        return left_val->visit(str_index->str);
      return nullptr;
    }
  }

private:
  AstNodePtr left;
  INDEX_TY index;
};
// Number literal or string literal
class Literal : public AstNode {
public:
  Literal(Token lit);
  ObjectBase *eval() override { return result_tmp; }
  ObjectBase *eval_with_create() override { return result_tmp->clone(); }
  std::string to_string() const override { return result_tmp->to_string(); }
  ~Literal() {
    // if (result_tmp)
    //   delete result_tmp;
  }

private:
  ObjectBase *result_tmp = nullptr;
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
  ObjectBase *eval() override;

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
  ObjectBase *eval() override;

private:
  std::vector<AstNodePtr> nodes;
};

} // namespace cake