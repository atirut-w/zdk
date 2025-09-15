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
  case Token::Plus:
    return "'+'";
  case Token::Asterisk:
    return "'*'";
  case Token::Slash:
    return "'/'";
  case Token::Percent:
    return "'%'";
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
  if (auto token = lexer.peek_token()) {
    if (token->kind == Token::Constant) {
      auto const_expr = std::make_unique<ConstantExpression>();
      const_expr->value = std::stoi(expect(Token::Constant).text);
      return const_expr;
    } else if (token->kind == Token::Tilde || token->kind == Token::Minus) {
      auto unary_expr = std::make_unique<UnaryExpression>();
      unary_expr->op = parse_unary_operator();
      unary_expr->operand = expression();
      return unary_expr;
    } else if (token->kind == Token::LeftParen) {
      expect(Token::LeftParen);
      auto expr = expression();
      expect(Token::RightParen);
      return expr;
    } else {
      throw Error(token->position, "Malformed expression");
    }
  } else {
    throw Error(lexer.position, "Unexpected end of input");
  }
}

UnaryExpression::Operator Parser::parse_unary_operator() {
  if (auto token = lexer.peek_token()) {
    if (token->kind == Token::Tilde) {
      expect(Token::Tilde);
      return UnaryExpression::Complement;
    } else if (token->kind == Token::Minus) {
      expect(Token::Minus);
      return UnaryExpression::Negate;
    } else {
      throw Error(token->position, "Expected unary operator");
    }
  } else {
    throw Error(lexer.position, "Unexpected end of input");
  }
}

} // namespace cparse
