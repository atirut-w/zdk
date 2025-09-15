#pragma once
#include "source_position.hpp"
#include <istream>
#include <optional>
#include "token.hpp"

namespace cparse {

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
