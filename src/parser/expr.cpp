#include <context.h>
#include <map>
#include <parser/expr.h>
#include <parser/function.h>
#include <parser/parser.h>
#include <parser/symbol.h>
#include <runtime/mem.h>
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
      {TokenKind::LE,{9,true}},
      {TokenKind::LT,{9,true}},
      {TokenKind::GE,{9,true}},
      {TokenKind::GT,{9,true}},
      {TokenKind::EQ,{10,false}},
      {TokenKind::NE,{10,false}},
      {TokenKind::ASSIGN, {16,false} }
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
  std::vector<Token> ops;
  nodes.push_back(std::move(left));
  while (!lexer.reach_to_end() && get_pred(lexer.peek(0).kind).first == ppred) {
    auto op = peek(0);
    lexer.next_token();

    ops.push_back(op);
    auto right = parse_expr_imp(ppred - 1);
    nodes.emplace_back(std::move(right));
  }

  if (nodes.size() == 1)
    return std::move(nodes.front());
  // left to right
  if (get_pred(ops.back().kind).second) {
    AstNodePtr root = std::move(nodes.front());
    for (int i = 1; i < nodes.size(); i++)
      root = make_unique<BinOp>(std::move(root), ops[i - 1].kind, std::move(nodes[i]));
    return root;
  } else {
    // right to left
    AstNodePtr root = std::move(nodes.back());
    // the size of ops = nodes.size()-1
    for (int i = ops.size() - 1; i >= 0; i--) {
      if (ppred == 16) {
        if (!nodes[i]->left_value())
          syntax_error("assign operation expect left value in the left side!", ops[i]);
        else
          root = make_unique<AssignOp>(std::move(nodes[i]), ops[i].kind, std::move(root));
      } else
        root = make_unique<BinOp>(std::move(nodes[i]), ops[i].kind, std::move(root));
    }
    return root;
  }
}
AstNodePtr Parser::parse_unit() {
  AstNodePtr left = nullptr;
  bool have_left = false;
  while (true) {
    auto tok = peek(0);
    switch (tok.kind) {
    case TokenKind::LSB: {
      match(LSB);
      // arrayVisitor
      if (have_left) {
        auto index = parse_expr();
        match(RSB);
        left = std::make_unique<ArrayVisit>(std::move(left), std::move(index));
        break;
      }
      // array literal
      std::vector<AstNodePtr> nodes;
      while (!lexer.reach_to_end() && lexer.peek(0).kind != RSB) {
        nodes.push_back(parse_expr());
        if (peek(0).kind != RSB)
          match(COMMA);
      }
      match(RSB);
      return make_unique<ArrayNode>(std::move(nodes));
    }
    case TokenKind::IDENTIFIER: {
      match(IDENTIFIER);
      if (left || have_left)
        cake_runtime_error("invalid expression!");
      have_left = true;

      auto sym = Context::global_symtab()->find_symbol(tok.text);
      if (!sym)
        syntax_error("undefined symbol " + std::string{tok.text}, tok);
      if (sym->get_kind() == SYM_VAR) {
        left = make_unique<Variable>(tok, static_cast<VarSymbol *>(sym)->get_stac_pos());
        break;
      } else if (sym->get_kind() == SYM_FUNC) {
        auto func = FunctionSymbol::get_func_def(sym);
        auto args = parse_expr_list(LPAR, RPAR);
        left = std::make_unique<CallNode>(tok, func, std::move(args));
        break;
      } else if (sym->get_kind() == SYM_CALLBLE) {
        auto callable = FunctionSymbol::get_callable(sym);
        auto args = parse_expr_list(LPAR, RPAR);
        left = std::make_unique<CallNode>(tok, callable, std::move(args));
        break;
      }
      syntax_error("unsupported symbol kind ");
      break;
    }
    // parse object
    case TokenKind::BEGIN: {
      match(BEGIN);
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
    case TokenKind::STRING:
    case TokenKind::INTEGER:
      lexer.next_token();
      if (have_left)
        cake_runtime_error("invalid literal!");
      return make_unique<cake::Literal>(tok);
    case TokenKind::LPAR: {
      match(LPAR);
      auto ret = parse_expr();
      match(TokenKind::RPAR);
      return ret;
    }
    default:
      return left;
    }
  }
}

ObjectBase *BinOp::eval() {
  auto lval = left->eval(), rval = right->eval();
#define BIN_OP_MP(TAG, OP)                                                                                             \
  case TokenKind::TAG: {                                                                                               \
    return lval->OP(rval);                                                                                             \
  }
  switch (op) {
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
  default:
    abort();
  }
}

Literal::Literal(Token lit) {
  if (lit.kind == TokenKind::INTEGER) {
    result_tmp = new NumberObject((int64_t)std::stoi(std::string{lit.text}));
  } else if (lit.kind == TokenKind::STRING) {
    auto str = utils::conv_escape(lit.text.substr(1, lit.text.size() - 2));
    if (!str)
      unreachable();
    result_tmp = new StringObject(*str);
  } else
    unreachable();
}

ObjectBase *AssignOp::eval() {
  auto ret = right->eval_with_create();
  switch (op) {
  case ASSIGN:
    *left->get_left_val() = right->eval_with_create();
    break;
  default:
    unreachable();
  }
  return ret;
}
std::string AssignOp::to_string() const {
  switch (op) {
  case ASSIGN:
    return "(ASSIGN " + left->to_string() + " " + right->to_string() + ")";
  default:
    return "(unknown assign op)";
  }
}
ObjectBase *Variable::eval() { return Memory::gmem.get_local(stac_pos); }

ObjectBase **Variable::get_left_val() { return &Memory::gmem.get_local(stac_pos); }

ObjectBase *ArrayNode::eval() {
  ArrayObject *ret;
  std::vector<ObjectBase *> arr;
  for (auto &node : nodes)
    arr.push_back(node->eval_with_create());
  return new ArrayObject(std::move(arr));
}

std::string ArrayVisit::to_string() const {
  return "(array_visit " + left->to_string() + " " + index->to_string() + ")";
}

ObjectBase *ArrayVisit::eval() {
  auto index_obj = index->eval_with_create();
  auto left_val = left->eval_with_create();
  auto ret = left_val->visit(NumberObject::get_integer_strict(index_obj));
  return ret;
}

ObjectBase **ArrayVisit::get_left_val() {
  auto index_obj = index->eval_with_create();
  auto left_val = left->eval_with_create();
  return &left_val->visit(NumberObject::get_integer_strict(index_obj));
}
} // namespace cake