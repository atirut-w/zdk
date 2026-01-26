#pragma once
#include "langkit/lexer.hpp"
#include "token.hpp"

class Lexer : public langkit::Lexer<Token> {
public:
  explicit Lexer(std::istream &in) : langkit::Lexer<Token>(in) {}

  std::optional<Token> next_token() override;
};
