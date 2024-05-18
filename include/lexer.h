#pragma once
#include <deque>
#include <string>
#include <string_view>
#include <vector>

namespace cake {
using std::string_view;

// clang-format off
enum TokenKind : uint8_t { 
  INTEGER, FLOAT, STRING,IDENTIFIER, 
  PLUS, MINUS, MUL, DIV, 
  SADD,SSUB,SMUL,SDIV, // +=,-=,*=,/=
  INC,DEC, // ++,--
  ASSIGN,
  LE,LT,GE,GT,EQ,NE, // compare operations
  AND,OR, // logical operations
  NOT,
  IF,ELSE,WHILE,FOR,SWITCH,BREAK,CASE,CONTINUE,
  LET,FUNCTION,CLASS,
  RETURN,
  LPAR, RPAR, LSB/*[*/, RSB, /*]*/ 
  BEGIN,END, // {}
  DOT,SEMI,COLON,COMMA, //., ; : ,
  NIL /*end of file or tokens*/
};

// clang-format on
struct Token {
  Token(TokenKind _kind, const std::string_view &val, uint32_t l, uint32_t c, uint32_t fno = 0)
      : kind(_kind), text(val), line(l), col(c), file_no(fno) {}

  static TokenKind get_word_kind(string_view word);
  static std::string token_kind_str(TokenKind kind);
  int64_t get_int() const { return std::stoi(std::string{text}); }
  std::string get_file_pos() const;
  std::string get_file_name() const;
  // if the token is string to get raw text.
  std::string string_raw_text() const;
  std::string_view text;

  TokenKind kind;
  // position information
  uint16_t file_no;
  uint32_t line;
  uint16_t col;
};

class Scanner {
public:
  Scanner(std::string _text, std::string filename);
  Scanner(std::string _text) : text(_text) {}

  Token next_token();
  Token peek(size_t);

  // lexer error
  [[noreturn]] void error(const std::string &msg);
  // mainly used for parser, using tok's line and col info
  [[noreturn]] void error(Token tok, const std::string &msg);
  std::string get_filename() const;
  bool reach_to_end() { return peek(0).kind == TokenKind::NIL; }
  int get_cur_line() const { return line; }

private:
  void skip_space();
  Token fetch_token();

  Token create_token(TokenKind kind, string_view str);

  Token scan_string_literal();

  int file_idx = 0;
  std::string text;
  // current position of text.
  size_t pos = 0;
  int line = 1;
  int col = 1;
  // to store some scanned tokens.
  std::deque<Token> buff;
};

} // namespace cake