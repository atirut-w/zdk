#pragma once
#include <fstream>
#include <optional>

class Token {
public:
  enum Type {
    PLUS,
    MINUS,
    STAR,
    SLASH,
    INTLIT,
  };
  Type type;
  int value;
};

class Lexer {
  std::ifstream &input;
  int line = 1;

  int get();
  void putback(int c);
  int skip();
  int chrpos(const char *s, int c);
  int scan_int(int c);

public:
  Lexer(std::ifstream &input) : input(input) {}
  std::optional<Token> next();
};
