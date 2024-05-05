#include <map>
#include <parser/expr.h>
#include <parser/parser.h>
#include <utils.h>

namespace cake {
using std::make_unique;

AstNodePtr Parser::parse_expr_imp(int ppred) {
  if (ppred == 1)
    return parse_unit();
  // clang-format off
  // <kind, precedance, is left to right.
  static std::map<TokenKind, std::pair<int, bool>> pred_tab = {
      {TokenKind::PLUS, {4, true}},
      {TokenKind::MINUS, {4, true}},
      {TokenKind::MUL, {3, true}},
      {TokenKind::DIV, {3, true}},
  };
  // clang-format on
  auto get_pred = [&](TokenKind tag) -> std::pair<int, bool> {
    auto it = pred_tab.find(tag);
    if (it == pred_tab.end())
      return {100, false};
    return it->second;
  };

  auto left = parse_expr_imp(ppred - 1);

  std::vector<AstNodePtr> nodes;
  std::vector<TokenKind> ops;
  nodes.push_back(std::move(left));
  while (!lexer.reach_to_end() && get_pred(lexer.peek(0).kind).first == ppred) {
    auto op = ctok_kind();
    lexer.next_token();

    ops.push_back(op);
    auto right = parse_expr_imp(ppred - 1);
    nodes.emplace_back(std::move(right));
  }

  if (nodes.size() == 1)
    return std::move(nodes.front());
  // left to right
  if (get_pred(ops.back()).second) {
    AstNodePtr root = std::move(nodes.front());
    for (int i = 1; i < nodes.size(); i++)
      root = make_unique<BinOp>(std::move(root), ops[i - 1], std::move(nodes[i]));
    return root;
  } else {
    // right to left
    AstNodePtr root = std::move(nodes.back());
    for (int i = ops.size() - 2; i >= 0; i--)
      root = make_unique<BinOp>(std::move(nodes[i]), ops[i], std::move(root));
    return root;
  }
}
AstNodePtr Parser::parse_unit() {
  auto tok = lexer.next_token();
  switch (tok.kind) {
  // parse array
  case TokenKind::LSB: {
    std::vector<AstNodePtr> nodes;
    while (!lexer.reach_to_end() && lexer.peek(0).kind != RSB) {
      nodes.push_back(parse_expr());
      if (peek(0).kind != RSB)
        match(COMMA);
    }
    match(RSB);
    return make_unique<ArrayNode>(std::move(nodes));
  }
  // parse object
  case TokenKind::BEGIN: {
    std::map<std::string, AstNodePtr> object_list;
    while (!lexer.reach_to_end() && lexer.peek(0).kind != END) {
      auto key = lexer.peek(0);
      // have not supported string key yet.
      match(IDENTIFIER);
      match(COLON);
      auto val = parse_expr();
      object_list.insert({std::string{key.text}, std::move(val)});
      if (peek(0).kind != END)
        match(COMMA);
    }
    match(END);
    return make_unique<ObjectNode>(std::move(object_list));
  }
  case TokenKind::INTEGER:
    return make_unique<cake::Literal>(tok);
  case TokenKind::LPAR: {
    auto ret = parse_expr();
    match(TokenKind::RPAR);
    return ret;
  }
  default:
    unreachable();
  }
}

ObjectBase *BinOp::eval() {
  auto lval = left->eval(), rval = left->eval();
  switch (op) {
  case TokenKind::PLUS: {
    auto res = lval->add(rval, result_tmp);
    if (result_tmp != res) {
      delete result_tmp;
      result_tmp = res;
    }
    break;
  }
  case TokenKind::MINUS: {
    auto res = lval->sub(rval, result_tmp);
    if (result_tmp != res) {
      delete result_tmp;
      result_tmp = res;
    }
    break;
  }
  case TokenKind::MUL: {
    auto res = lval->mul(rval, result_tmp);
    if (result_tmp != res) {
      delete result_tmp;
      result_tmp = res;
    }
    break;
  }
  case TokenKind::DIV: {
    auto res = lval->div(rval, result_tmp);
    if (result_tmp != res) {
      delete result_tmp;
      result_tmp = res;
    }
    break;
  }
  default:
    abort();
  }
  return result_tmp;
}

Literal::Literal(Token lit) {
  if (lit.kind == TokenKind::INTEGER) {
    result_tmp = new NumberObject((int64_t)std::stoi(std::string{lit.text}));
  } else
    unreachable();
}
} // namespace cake