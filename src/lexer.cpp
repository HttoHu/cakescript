#include <context.h>
#include <fmt/format.h>
#include <iostream>
#include <lexer.h>
#include <unordered_map>

namespace cake {

TokenKind Token::get_word_kind(string_view word) {
  const std::unordered_map<string_view, TokenKind> tab = {{"if", TokenKind::IF},
                                                          {"while", TokenKind::WHILE},
                                                          {"for", TokenKind::FOR},
                                                          {"let", TokenKind::LET},
                                                          {"else", TokenKind::ELSE},
                                                          {"function", TokenKind::FUNCTION}};
  auto it = tab.find(word);
  if (it != tab.end())
    return it->second;
  return TokenKind::IDENTIFIER;
}
std::string Token::token_kind_str(TokenKind kind) {
  using enum TokenKind;
  switch (kind) {
  case PLUS:
    return "PLUS";
  case MINUS:
    return "MINUS";
  case MUL:
    return "MUL";
  case DIV:
    return "DIV";
  case ASSIGN:
    return "ASSIGN";
  case INTEGER:
    return "INTEGER";
  case EQ:
    return "EQ";
  case NE:
    return "NE";
  default:
    return "UNKNOWN";
  }
}
std::string Token::get_file_pos() const {
  return fmt::format("{}:{}:{}", Context::global_context()->get_source_file(file_no), line, col);
}

std::string Token::get_file_name() const { return Context::global_context()->get_source_file(file_no); }

Scanner::Scanner(std::string _text, std::string filename) : text(_text) {
  file_idx = Context::global_context()->add_source_file(filename);
}

Token Scanner::next_token() {
  if (buff.size()) {
    auto ret = buff.front();
    buff.pop_front();
    return ret;
  }
  return fetch_token();
}
Token Scanner::peek(size_t idx) {
  while (idx >= buff.size())
    buff.push_back(fetch_token());
  return buff[idx];
}

void Scanner::error(const std::string &msg) {
  std::cerr << fmt::format("{}:{}:{}: lexer error: {}", get_filename(), line, col, msg) << std::endl;
  exit(1);
}
void Scanner::error(Token tok, const std::string &msg) {
  std::cerr << fmt::format("{}:{}:{}: {}", tok.get_file_name(), tok.line, tok.col, msg) << std::endl;
  exit(1);
}
std::string Scanner::get_filename() const { return Context::global_context()->get_source_file(file_idx); }
Token Scanner::fetch_token() {
  skip_space();
  if (pos >= text.size())
    return create_token(TokenKind::NIL, "");
  auto ch = text[pos];
  pos++, col++;
  switch (ch) {
  case '+':
    return create_token(TokenKind::PLUS, "+");
  case '-':
    return create_token(TokenKind::MINUS, "-");
  case '*':
    return create_token(TokenKind::MUL, "*");
  case '/':
    return create_token(TokenKind::DIV, "/");
  case '(':
    return create_token(TokenKind::LPAR, "(");
  case ')':
    return create_token(TokenKind::RPAR, ")");
  case '[':
    return create_token(TokenKind::LSB, "[");
  case ']':
    return create_token(TokenKind::RSB, "]");
  case '{':
    return create_token(TokenKind::BEGIN, "{");
  case '}':
    return create_token(TokenKind::END, "}");
  case ',':
    return create_token(TokenKind::COMMA, ",");
  case ':':
    return create_token(TokenKind::COLON, ":");
  case ';':
    return create_token(TokenKind::SEMI, ";");
  case '<':
    if (pos < text.size() && text[pos] == '=' && ++pos)
      return create_token(TokenKind::LE, "<=");
    return create_token(TokenKind::LT, "<");
  case '>':
    if (pos < text.size() && text[pos] == '=' && ++pos)
      return create_token(TokenKind::GE, ">=");
    return create_token(TokenKind::GT, ">");
  case '!':
    if (pos < text.size() && text[pos] == '=' && ++pos)
      return create_token(TokenKind::NE, "!=");
    return create_token(TokenKind::NOT, "!");
  case '=': {
    if (pos < text.size() && text[pos] == '=' && ++pos)
      return create_token(TokenKind::EQ, "==");
    return create_token(TokenKind::ASSIGN, "=");
  }
  default: {
    if (isdigit(ch)) {
      int len = 1;
      while (isdigit(text[pos])) {
        pos++, col++, len++;
      }
      return create_token(TokenKind::INTEGER, string_view{text}.substr(pos - len, len));
    } else if (isalpha(ch) || text[pos] == '_') {
      int len = 1;
      while (isalnum(text[pos]) || text[pos] == '_') {
        pos++, col++, len++;
      }
      string_view slice = string_view{text}.substr(pos - len, len);
      return create_token(Token::get_word_kind(slice), slice);
    } else {
      error(fmt::format("unexpected charater {}", ch));
    }
  }
  }
}

Token Scanner::create_token(TokenKind kind, string_view str) {
  int l = line, c = col;

  for (int i = str.size() - 1; i >= 0; i--) {
    if (str[i] != '\n')
      c--;
    else
      l--; // we can't calculate col if we meet a newline
  }
  return Token(kind, str, l, std::max(1, c), file_idx);
}
void Scanner::skip_space() {
  // skip space
  while (pos < text.size() && isspace(text[pos])) {
    if (text[pos] == '\n')
      line++, col = 0;

    col++;
    pos++;
  }
  // line comment
  if (pos + 1 < text.size() && text[pos] == '/' && text[pos + 1] == '/') {
    while (pos < text.size() && text[pos] != '\n')
      pos++;
    line++, col = 1;
    skip_space();
  }
  // cross line comment
  if (pos + 1 < text.size() && text[pos] == '/' && text[pos + 1] == '*') {
    pos += 2, col += 2;
    bool have_termilator = false;
    while (pos + 1 < text.size()) {
      if (text[pos] == '*' && text[pos + 1] == '/') {
        pos += 2, col += 2;
        have_termilator = true;
        break;
      }
      if (text[pos] == '\n')
        line++, col = 1;
      pos++, col++;
    }
    if (!have_termilator)
      error("expect comment terminator \"*/\" !");
    skip_space();
  }
}
} // namespace cake