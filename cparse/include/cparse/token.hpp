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

    Semicolon,
    LeftBrace,
    RightBrace,
    LeftParen,
    RightParen,
    Tilde,
    Minus,
    MinusMinus,
  };

  Kind kind;
  SourcePosition position;
  std::string text;
};

} // namespace cparse
