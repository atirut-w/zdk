#pragma once
#include <cstddef>
#include <istream>
#include <optional>

namespace langkit {

/**
 * Language-agnostic base class for lexers.
 */
template <typename TokenType, int BufferSize = 64> class Lexer {
  std::istream &input;

public:
  explicit Lexer(std::istream &in) : input(in) {}
  virtual ~Lexer() = default;

  /**
   * Gets the next token from the input stream. Returns std::nullopt on EOF.
   */
  virtual std::optional<TokenType> next_token() = 0;
};

} // namespace langkit
