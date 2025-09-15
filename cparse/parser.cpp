#include "cparse/parser.hpp"
#include "cparse/error.hpp"

namespace cparse {

static std::string kind_name(Token::Kind kind) {
  switch (kind) {
  case Token::Int:
    return "'int'";
  case Token::Return:
    return "'return'";
  case Token::Void:
    return "'void'";
  case Token::Identifier:
    return "identifier";
  case Token::Constant:
    return "constant";
  case Token::MinusMinus:
    return "'--'";
  case Token::Semicolon:
    return "';'";
  case Token::LeftBrace:
    return "'{'";
  case Token::RightBrace:
    return "'}'";
  case Token::LeftParen:
    return "'('";
  case Token::RightParen:
    return "')'";
  case Token::Tilde:
    return "'~'";
  case Token::Minus:
    return "'-'";
  default:
    throw std::runtime_error("BUG: Unhandled token kind");
  }
}

Token Parser::expect(Token::Kind kind) {
  if (auto token = lexer.peek_token()) {
    if (token->kind == kind) {
      lexer.next();
      return *token;
    } else {
      throw Error(token->position,
                  std::format("Expected {}, got {}", kind_name(kind),
                              kind_name(token->kind)));
    }
  } else {
    throw Error(lexer.position, "Unexpected end of input");
  }
}

std::unique_ptr<TranslationUnit> Parser::translation_unit() {
  auto tu = std::make_unique<TranslationUnit>();
  tu->function = function_definition();
  return tu;
}

std::unique_ptr<FunctionDefinition> Parser::function_definition() {
  auto func = std::make_unique<FunctionDefinition>();
  expect(Token::Int);
  func->name = expect(Token::Identifier).text;
  expect(Token::LeftParen);
  expect(Token::Void);
  expect(Token::RightParen);
  expect(Token::LeftBrace);
  func->body = statement();
  expect(Token::RightBrace);

  return func;
}

std::unique_ptr<Statement> Parser::statement() { return return_statement(); }

std::unique_ptr<ReturnStatement> Parser::return_statement() {
  auto ret = std::make_unique<ReturnStatement>();
  expect(Token::Return);
  ret->expression = expression();
  expect(Token::Semicolon);
  return ret;
}

std::unique_ptr<Expression> Parser::expression() {
  auto expr = std::make_unique<ConstantExpression>();
  expr->value = std::stoi(expect(Token::Constant).text);
  return expr;
}

} // namespace cparse
