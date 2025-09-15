#include "cparse/lexer.hpp"
#include <cparse/error.hpp>
#include <format>
#include <unordered_map>

namespace cparse {

static std::unordered_map<std::string, Token::Kind> keywords = {
    {"int", Token::Int},
    {"return", Token::Return},
    {"void", Token::Void},
};

std::optional<Token> Lexer::next() {
  while (isspace(peek())) {
    consume();
  }

  current.position = position;
  current.text.clear();

  if (isalpha(peek()) || peek() == '_') {
    while (isalnum(peek()) || peek() == '_') {
      current.text += consume();
    }

    if (!at_boundary()) {
      throw Error(current.position,
                  std::format("Invalid character '{}' in identifier", peek()));
    }

    if (auto it = keywords.find(current.text); it != keywords.end()) {
      current.kind = it->second;
    } else {
      current.kind = Token::Identifier;
    }

    return current;
  } else if (isdigit(peek())) {
    while (isdigit(peek())) {
      current.text += consume();
    }

    if (!at_boundary()) {
      throw Error(current.position,
                  std::format("Invalid character '{}' in constant", peek()));
    }

    current.kind = Token::Constant;
    return current;
  } else {
    switch (peek()) {
    case ';':
      consume();
      current.kind = Token::Semicolon;
      return current;
    case '{':
      consume();
      current.kind = Token::LeftBrace;
      return current;
    case '}':
      consume();
      current.kind = Token::RightBrace;
      return current;
    case '(':
      consume();
      current.kind = Token::LeftParen;
      return current;
    case ')':
      consume();
      current.kind = Token::RightParen;
      return current;
    case '~':
      consume();
      current.kind = Token::Tilde;
      return current;
    case '-':
      consume();
      if (peek() == '-') {
        consume();
        current.kind = Token::MinusMinus;
      } else {
        current.kind = Token::Minus;
      }
      return current;
    case '+':
      consume();
      current.kind = Token::Plus;
      return current;
    case '*':
      consume();
      current.kind = Token::Asterisk;
      return current;
    case '/':
      consume();
      current.kind = Token::Slash;
      return current;
    case '%':
      consume();
      current.kind = Token::Percent;
      return current;
    case '\0':
      return std::nullopt;
    default:
      throw Error(current.position,
                  std::format("Invalid character '{}'", peek()));
    }
  }

  return std::nullopt;
}

std::optional<Token> Lexer::peek_token() {
  auto saved_position = position;
  auto saved_current = current;
  auto token = next();
  position = saved_position;
  current = saved_current;
  input.clear();
  input.seekg(position.offset);
  return token;
}

char Lexer::consume() {
  int ch = input.get();
  if (ch == '\n') {
    // TODO: Handle other line endings
    position.line++;
    position.column = 1;
  } else {
    position.column++;
  }
  position.offset++;
  return ch == EOF ? '\0' : static_cast<char>(ch);
}

char Lexer::peek() const {
  return input.peek() == EOF ? '\0' : static_cast<char>(input.peek());
}

bool Lexer::at_boundary() const { return !isalnum(peek()) && peek() != '_'; }

} // namespace cparse
