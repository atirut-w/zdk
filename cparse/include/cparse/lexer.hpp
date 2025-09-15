#pragma once
#include "source_position.hpp"
#include <istream>
#include <optional>

namespace cparse {

struct Token {
  enum Kind {
    Int,
    Return,
    Void,

    Identifier,
    Constant,

    Semicolon,
    LeftBrace,
    RightBrace,
    LeftParen,
    RightParen,
  };

  Kind kind;
  SourcePosition position;
  std::string text;
};

struct Lexer {
  std::istream &input;
  SourcePosition position;
  Token current;

  std::optional<Token> next();
  char consume();
  char peek() const;
  bool at_boundary() const;
};

} // namespace cparse
