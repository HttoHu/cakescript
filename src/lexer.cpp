#include <context.h>
#include <fmt/format.h>
#include <iostream>
#include <lexer.h>
#include <map>
#include <unordered_map>
#include <utils.h>

namespace cake {

TokenKind Token::get_word_kind(string_view word) {
  const std::unordered_map<string_view, TokenKind> tab = {
      {"if", TokenKind::IF},        {"while", TokenKind::WHILE}, {"for", TokenKind::FOR},
      {"let", TokenKind::LET},      {"else", TokenKind::ELSE},   {"function", TokenKind::FUNCTION},
      {"return", TokenKind::RETURN}};
  auto it = tab.find(word);
  if (it != tab.end())
    return it->second;
  return TokenKind::IDENTIFIER;
}
std::string Token::token_kind_str(TokenKind kind) {
  using enum TokenKind;
#define __TMP_MATH(KIND)                                                                                               \
  case KIND:                                                                                                           \
    return #KIND;
  switch (kind) {
    __TMP_MATH(PLUS)
    __TMP_MATH(MINUS)
    __TMP_MATH(MUL)
    __TMP_MATH(DIV)
    __TMP_MATH(ASSIGN)
    __TMP_MATH(INTEGER)
    __TMP_MATH(EQ)
    __TMP_MATH(NE)
    __TMP_MATH(LT)
    __TMP_MATH(LE)
    __TMP_MATH(GE)
    __TMP_MATH(GT)
    __TMP_MATH(LSH)
    __TMP_MATH(RSH)
    __TMP_MATH(AND)
    __TMP_MATH(OR)
  default:
    return "UNKNOWN";
  }
#undef __TMP_MATH
}
std::string Token::get_file_pos() const {
  return fmt::format("{}:{}:{}", Context::global_context()->get_source_file(file_no), line, col);
}

std::string Token::get_file_name() const { return Context::global_context()->get_source_file(file_no); }

std::string Token::string_raw_text() const {
  if (kind != STRING)
    throw std::runtime_error("internel error parse string failed, not a string token!");
  auto res = utils::conv_escape(text.substr(1, text.size() - 2));
  // scanner should make sure the string literal is illegal.
  if (!res)
    throw std::runtime_error("internel error parse string failed!");
  return *res;
}
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
#define TEST_CHAR(ch, TAG, TEXT)                                                                                       \
  if (pos < text.size() && text[pos] == ch && ++pos)                                                                   \
    return create_token(TokenKind::TAG, TEXT);

  switch (ch) {
  case '+':
    TEST_CHAR('=', SADD, "+=")
    TEST_CHAR('+', INC, "++")
    return create_token(TokenKind::PLUS, "+");
  case '-':
    TEST_CHAR('=', SADD, "-=")
    TEST_CHAR('-', DEC, "--")
    return create_token(TokenKind::MINUS, "-");
  case '*':
    TEST_CHAR('=', SMUL, "*=")
    return create_token(TokenKind::MUL, "*");
  case '/':
    TEST_CHAR('=', SMUL, "/=")
    return create_token(TokenKind::DIV, "/");
  case '%':
    return create_token(TokenKind::MOD, "%");
  case '&':
    TEST_CHAR('&', AND, "&&")
    return create_token(TokenKind::BIT_AND, "&");
  case '|':
    TEST_CHAR('|', OR, "||")
    return create_token(TokenKind::BIT_OR, "|");
  case '^':
    return create_token(TokenKind::BIT_XOR, "^");
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
  case '.':
    return create_token(TokenKind::DOT, ".");
  case ',':
    return create_token(TokenKind::COMMA, ",");
  case ':':
    return create_token(TokenKind::COLON, ":");
  case ';':
    return create_token(TokenKind::SEMI, ";");
  case '\"':
    return scan_string_literal();
  case '<':
    TEST_CHAR('=', LE, "<=")
    else TEST_CHAR('<', LSH, "<<") return create_token(TokenKind::LT, "<");
  case '>':
    TEST_CHAR('=', GE, ">=")
    else TEST_CHAR('>', RSH, ">>") return create_token(TokenKind::GT, ">");
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
#undef TEST_CHAR
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

Token Scanner::scan_string_literal() {
  // jump over "
  int l = pos - 1, c = col - 1;
  for (; pos < text.size(); pos++, col++) {
    if (text[pos] == '\"')
      break;

    if (text[pos] == '\\') {
      pos++, col++;
    } else if (text[pos] == '\n')
      error("scan string but got a newline!");
  }
  // now str[pos] expected "
  int r = pos;
  if (text[pos] != '\"')
    error("string literal terminaltor not found");
  pos++;
  return Token(TokenKind::STRING, std::string_view(text).substr(l, r - l + 1), line, c, file_idx);
}
} // namespace cake