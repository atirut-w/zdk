#include "lexer.hpp"
#include <cctype>
#include <cstdio>
#include <cstring>

using namespace std;

int Lexer::get() {
  int c = input.get();
  if (c == '\n') {
    line++;
  }
  return c;
}

void Lexer::putback(int c) { input.putback(c); }

int Lexer::skip() {
  int c = get();
  while (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f') {
    c = get();
  }
  return c;
}

int Lexer::chrpos(const char *s, int c) {
  const char *p = strchr(s, c);
  return p ? p - s : -1;
}

int Lexer::scan_int(int c) {
  int k, val = 0;

  while ((k = chrpos("0123456789", c)) >= 0) {
    val = val * 10 + k;
    c = get();
  }

  putback(c);
  return val;
}

optional<Token> Lexer::next() {
  Token token;
  int c = skip();

  switch (c) {
  default:
    if (isdigit(c)) {
      token.type = Token::INTLIT;
      token.value = scan_int(c);
      return token;
    }
    throw runtime_error("unrecognized character " + to_string(c) + " on line " + to_string(line));
  case EOF:
    return nullopt;
  case '+':
    token.type = Token::PLUS;
    return token;
  case '-':
    token.type = Token::MINUS;
    return token;
  case '*':
    token.type = Token::STAR;
    return token;
  case '/':
    token.type = Token::SLASH;
    return token;
  }
}
