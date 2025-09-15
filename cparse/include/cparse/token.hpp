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
    AndOp,
    OrOp,
    LeOp,
    GeOp,
    EqOp,
    NeOp,
    Semicolon,
    LeftBrace,
    RightBrace,
    Equal,
    LeftParen,
    RightParen,
    Exclamation,
    Tilde,
    Minus,
    Plus,
    Asterisk,
    Slash,
    Percent,
    LeftAngle,
    RightAngle,
  };

  Kind kind;
  SourcePosition position;
  std::string text;
};

} // namespace cparse
