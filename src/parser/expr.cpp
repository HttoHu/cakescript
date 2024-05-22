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
    syntax_error("unsupported expression!");

  if (ppred == 2)
    return parse_unit();
  else if (ppred == 3)
    return parse_unary();
  // clang-format off
  // <kind, precedance, is left to right.
  // although it is a JS-like interpreter, the precedence refered from 
  // https://en.cppreference.com/w/cpp/language/operator_precedence
  static std::map<TokenKind, std::pair<int, bool>> pred_tab = {
      {TokenKind::INC,{3,false}},
      {TokenKind::DEC,{3,false}}, // in cakescript, DEC,INC don't have Associativity, you can't write - -a,or -- -- a
      {TokenKind::MUL, {5, true}},
      {TokenKind::DIV, {5, true}},
      {TokenKind::MOD, {5, true}},
      {TokenKind::PLUS, {6, true}},
      {TokenKind::MINUS, {6, true}}, // binary operation minus
      {TokenKind::LSH,{7,true}},{TokenKind::RSH,{7,true}},
      {TokenKind::LE,{9,true}},{TokenKind::LT,{9,true}},{TokenKind::GE,{9,true}},{TokenKind::GT,{9,true}},
      {TokenKind::EQ,{10,false}},{TokenKind::NE,{10,false}},
      {TokenKind::BIT_AND,{11,true}},{TokenKind::BIT_XOR,{12,true}},{TokenKind::BIT_OR,{13,true}},
      {TokenKind::AND,{14,true}},{TokenKind::OR,{15,true}},
      {TokenKind::ASSIGN, {16,false} },
      {TokenKind::SADD, {16,false} },
      {TokenKind::SSUB, {16,false} },
      {TokenKind::SMUL, {16,false} },
      {TokenKind::SDIV, {16,false} },
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
  int line_no = lexer.get_cur_line();
  while (true) {
    auto tok = peek(0);
    // unit can not cross line
    if (tok.line != line_no)
      break;

    switch (tok.kind) {
    case TokenKind::DOT: {
      match(DOT);
      if (!have_left)
        syntax_error("member visit must have a left value!");
      auto id = lexer.peek(0);
      match(IDENTIFIER);
      // call member function
      auto old_left = left.get();
      left = std::make_unique<VistorMember<std::string>>(std::move(left), std::string{id.text});
      if (peek(0).kind == LPAR) {
        auto args = parse_expr_list(LPAR, RPAR);
        left = std::make_unique<CallMemberFunction>(std::move(left), old_left, std::move(args));
      }
      break;
    }
    case TokenKind::LSB: {
      match(LSB);
      // arrayVisitor
      if (have_left) {
        if (peek(0).kind == INTEGER) {
          auto index_tok = lexer.next_token();
          left = std::make_unique<VistorMember<int64_t>>(std::move(left), index_tok.get_int());
        } else if (peek(0).kind == STRING) {
          auto index_tok = lexer.next_token();
          left = std::make_unique<VistorMember<std::string>>(std::move(left), std::string{index_tok.text});
        } else {
          left = std::make_unique<VistorMember<AstNodePtr>>(std::move(left), parse_expr());
        }
        match(RSB);
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
      // left identifier can only defined once
      if (left || have_left)
        syntax_error("invalid expression current token: " + std::string{peek(0).text});
      have_left = true;

      auto sym = Context::global_symtab()->find_symbol(tok.text);
      if (!sym) {
        if (peek(0).kind == LPAR) // functions defined after call node
        {
          auto args = parse_expr_list(LPAR, RPAR);
          left = std::make_unique<CallNode>(tok, std::move(args));
          Context::global_context()->global_symtab()->add_undef_func_call(static_cast<CallNode *>(left.get()));
          break;
        } else
          syntax_error("undefined symbol " + std::string{tok.text}, tok);
      }
      if (sym->get_kind() == SYM_VAR) {
        if (sym->is_global())
          left = make_unique<Variable<true>>(tok, static_cast<VarSymbol *>(sym)->get_stac_pos());
        else
          left = make_unique<Variable<false>>(tok, static_cast<VarSymbol *>(sym)->get_stac_pos());
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
        auto key = lexer.next_token();
        // have not supported string key yet.
        std::string key_val;
        if (key.kind == IDENTIFIER)
          key_val = key.text;
        else if (key.kind == STRING)
          key_val = key.string_raw_text();
        else
          cake_runtime_error("list init expected string or identifer as key!");

        match(COLON);
        auto val = parse_expr();

        object_list.insert({key_val, std::move(val)});
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
    case TokenKind::DEC:
    case TokenKind::INC: {
      if (!left)
        syntax_error("unexpected INC!");
      if (dynamic_cast<Literal *>(left.get()))
        syntax_error("literal can not dec or inc!");
      lexer.next_token();
      if (tok.kind == INC)
        return std::make_unique<PostIncDec<true>>(std::move(left));
      return std::make_unique<PostIncDec<false>>(std::move(left));
    }
    default:
      if (!left)
        syntax_error("parse unit failed!");
      return left;
    }
  }
  if (!left)
    syntax_error("parse unit failed!");
  return left;
}

AstNodePtr Parser::parse_unary() {
  auto op_kind = peek(0).kind;
  switch (op_kind) {
  case NOT:
    lexer.next_token();
    return std::make_unique<UnaryOp<NOT>>(op_kind, parse_unit());
  case INC:
  case DEC: {
    lexer.next_token();
    auto expr = parse_unit();
    if (!expr->left_value())
      syntax_error("the expr of inc/dec must be left value!");
    if (op_kind == DEC)
      return std::make_unique<UnaryOp<DEC>>(op_kind, std::move(expr));
    else
      return std::make_unique<UnaryOp<INC>>(op_kind, std::move(expr));
  }
  case MINUS:
    lexer.next_token();
    return std::make_unique<UnaryOp<MINUS>>(op_kind, parse_unit());

  default:
    // not an unary node.
    return parse_unit();
  }
}

ObjectBase *BinOp::eval_with_create() {
  auto lval = left->eval();
  auto rval = right->eval();
#define BIN_OP_MP(TAG, OP)                                                                                             \
  case TokenKind::TAG: {                                                                                               \
    return lval->OP(rval.get());                                                                                       \
  }
#define INT_BIN_OP(TAG, OP)                                                                                            \
  case TokenKind::TAG: {                                                                                               \
    auto l = dynamic_cast<IntegerObject *>(lval.get()), r = dynamic_cast<IntegerObject *>(rval.get());                 \
    if (!l || !r)                                                                                                      \
      cake_runtime_error("bitwise op: expect integer operand!");                                                       \
    return new IntegerObject(l->get_int() OP r->get_int());                                                            \
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
    INT_BIN_OP(LSH, <<)
    INT_BIN_OP(MOD, %)
    INT_BIN_OP(RSH, >>)
    INT_BIN_OP(BIT_AND, &)
    INT_BIN_OP(BIT_OR, |)
    INT_BIN_OP(BIT_XOR, ^)
    INT_BIN_OP(AND, &&)
    INT_BIN_OP(OR, ||)
  default:
    cake_runtime_error("bianry op: unsupported operation!");
  }
}
TmpObjectPtr BinOp::eval() { return TmpObjectPtr(eval_with_create(), true); }
Literal::Literal(Token lit) {
  if (lit.kind == TokenKind::INTEGER) {
    result_tmp = new IntegerObject((int64_t)std::stoi(std::string{lit.text}));
  } else if (lit.kind == TokenKind::STRING) {
    auto str = utils::conv_escape(lit.text.substr(1, lit.text.size() - 2));
    if (!str)
      unreachable();
    result_tmp = new StringObject(*str);
  } else
    cake_runtime_error("unknown literal");
}

TmpObjectPtr AssignOp::eval() {
  auto left_val = left->get_left_val();
  switch (op) {
  // the object may be reassign an differnt type object, so we need delete old object
  case ASSIGN: {
    TmpObjectPtr t(*left_val, true);
    // debug for a long time: right may use left_val, we can't delete left_val before right->eval_with_create()
    *left_val = right->eval_with_create();
    return *left_val;
  }
  case SADD: {
    (*left_val)->sadd(right->eval().get());
    break;
  case SSUB:
    (*left_val)->ssub(right->eval().get());
    break;
  case SMUL:
    (*left_val)->smul(right->eval().get());
    break;
  case SDIV:
    (*left_val)->sdiv(right->eval().get());
    break;
  }
  default:
    unreachable();
  }
  return *left_val;
}
std::string AssignOp::to_string() const {
  switch (op) {
  case ASSIGN:
    return "(ASSIGN " + left->to_string() + " " + right->to_string() + ")";
  default:
    return "(unknown assign op)";
  }
}

TmpObjectPtr ObjectNode::eval() {
  std::vector<std::pair<std::string, ObjectBase *>> tab;
  for (auto &[k, v] : object)
    tab.push_back({k, v->eval_with_create()});
  return TmpObjectPtr(new StructObject(std::move(tab)), true);
}

TmpObjectPtr ArrayNode::eval() {
  std::vector<ObjectBase *> arr;
  for (auto &node : nodes)
    arr.push_back(node->eval_with_create());
  return TmpObjectPtr(new ArrayObject(std::move(arr)), true);
}

} // namespace cake