#pragma once
#include "source_position.hpp"
#include <string>

namespace cparse {

struct Token {
  enum Kind {
    Int,
    Return,
    Void,

    Identifier,
    Constant,

    MinusMinus,
    Semicolon,
    LeftBrace,
    RightBrace,
    LeftParen,
    RightParen,
    Tilde,
    Minus,
  };

  Kind kind;
  SourcePosition position;
  std::string text;
};

} // namespace cparse
