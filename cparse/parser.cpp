#include "cparse/parser.hpp"
#include "cparse/error.hpp"
#include <unordered_map>
#include <vector>

namespace cparse {

static std::unordered_map<Token::Kind, int> precedence = {
    {Token::Asterisk, 50},   {Token::Slash, 50}, {Token::Percent, 50},
    {Token::Plus, 45},       {Token::Minus, 45}, {Token::LeftAngle, 35},
    {Token::RightAngle, 35}, {Token::LeOp, 35},  {Token::GeOp, 35},
    {Token::EqOp, 30},       {Token::NeOp, 30},  {Token::AndOp, 10},
    {Token::OrOp, 5},        {Token::Equal, 1},
};

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
  case Token::AndOp:
    return "'&&'";
  case Token::OrOp:
    return "'||'";
  case Token::LeOp:
    return "'<='";
  case Token::GeOp:
    return "'>='";
  case Token::EqOp:
    return "'=='";
  case Token::NeOp:
    return "'!='";
  case Token::Semicolon:
    return "';'";
  case Token::LeftBrace:
    return "'{'";
  case Token::RightBrace:
    return "'}'";
  case Token::Equal:
    return "'='";
  case Token::LeftParen:
    return "'('";
  case Token::RightParen:
    return "')'";
  case Token::Exclamation:
    return "'!'";
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
  case Token::LeftAngle:
    return "'<'";
  case Token::RightAngle:
    return "'>'";
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

  // Declarations
  while (auto token = lexer.peek_token()) {
    if (token->kind == Token::RightBrace) {
      break;
    } else if (token->kind == Token::Int) {
      func->declarations.push_back(declaration());
    } else {
      break;
    }
  }

  // Statements
  while (auto token = lexer.peek_token()) {
    if (token->kind == Token::RightBrace) {
      break;
    } else if (token->kind == Token::Int) {
      // Error: declaration after statements in C90
      throw Error(token->position,
                  "Declaration not allowed after statements in C90");
    } else {
      func->body.push_back(statement());
    }
  }

  expect(Token::RightBrace);

  return func;
}

std::unique_ptr<Declaration> Parser::declaration() {
  auto decl = std::make_unique<Declaration>();
  expect(Token::Int);
  decl->name = expect(Token::Identifier).text;
  if (auto token = lexer.peek_token(); token && token->kind == Token::Equal) {
    expect(Token::Equal);
    decl->initializer = expression();
  }
  expect(Token::Semicolon);
  return decl;
}

std::unique_ptr<Statement> Parser::statement() {
  if (auto token = lexer.peek_token()) {
    if (token->kind == Token::Return) {
      return return_statement();
    } else {
      return expression_statement();
    }
  } else {
    throw Error(lexer.position, "Unexpected end of input");
  }
}

std::unique_ptr<Statement> Parser::expression_statement() {
  auto expr_stmt = std::make_unique<ExpressionStatement>();
  expr_stmt->expression = expression();
  expect(Token::Semicolon);
  return expr_stmt;
}

std::unique_ptr<ReturnStatement> Parser::return_statement() {
  auto ret = std::make_unique<ReturnStatement>();
  expect(Token::Return);
  ret->expression = expression();
  expect(Token::Semicolon);
  return ret;
}

std::unique_ptr<Expression> Parser::factor() {
  if (auto token = lexer.peek_token()) {
    if (token->kind == Token::Constant) {
      auto const_expr = std::make_unique<ConstantExpression>();
      const_expr->value = std::stoi(expect(Token::Constant).text);
      return const_expr;
    } else if (token->kind == Token::Identifier) {
      auto id_expr = std::make_unique<IdentifierExpression>();
      id_expr->name = expect(Token::Identifier).text;
      return id_expr;
    } else if (token->kind == Token::Tilde || token->kind == Token::Minus) {
      auto unary_expr = std::make_unique<UnaryExpression>();
      unary_expr->op = unary_operator();
      unary_expr->operand = factor();
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

std::unique_ptr<Expression> Parser::expression(int min_prec) {
  auto lhs = factor();
  auto next = lexer.peek_token();

  while (next && precedence.contains(next->kind) &&
         precedence[next->kind] >= min_prec) {
    if (next->kind == Token::Equal) {
      expect(Token::Equal);
      auto rhs = expression(precedence[next->kind]);
      auto assign_expr = std::make_unique<AssignmentExpression>();
      assign_expr->left = std::move(lhs);
      assign_expr->right = std::move(rhs);
      lhs = std::move(assign_expr);
    } else {
      auto op = binary_operator();
      auto rhs = expression(precedence[next->kind] + 1);
      auto bin_expr = std::make_unique<BinaryExpression>();
      bin_expr->op = op;
      bin_expr->left = std::move(lhs);
      bin_expr->right = std::move(rhs);
      lhs = std::move(bin_expr);
    }
    next = lexer.peek_token();
  }

  return lhs;
}

UnaryExpression::Operator Parser::unary_operator() {
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

BinaryExpression::Operator Parser::binary_operator() {
  if (auto token = lexer.peek_token()) {
    switch (token->kind) {
    case Token::Plus:
      expect(Token::Plus);
      return BinaryExpression::Add;
    case Token::Minus:
      expect(Token::Minus);
      return BinaryExpression::Subtract;
    case Token::Asterisk:
      expect(Token::Asterisk);
      return BinaryExpression::Multiply;
    case Token::Slash:
      expect(Token::Slash);
      return BinaryExpression::Divide;
    case Token::Percent:
      expect(Token::Percent);
      return BinaryExpression::Modulus;
    case Token::AndOp:
      expect(Token::AndOp);
      return BinaryExpression::And;
    case Token::OrOp:
      expect(Token::OrOp);
      return BinaryExpression::Or;
    case Token::EqOp:
      expect(Token::EqOp);
      return BinaryExpression::Equal;
    case Token::NeOp:
      expect(Token::NeOp);
      return BinaryExpression::NotEqual;
    case Token::LeftAngle:
      expect(Token::LeftAngle);
      return BinaryExpression::Less;
    case Token::LeOp:
      expect(Token::LeOp);
      return BinaryExpression::LessEqual;
    case Token::RightAngle:
      expect(Token::RightAngle);
      return BinaryExpression::Greater;
    case Token::GeOp:
      expect(Token::GeOp);
      return BinaryExpression::GreaterEqual;
    default:
      throw Error(token->position, "Expected binary operator");
    }
  } else {
    throw Error(lexer.position, "Unexpected end of input");
  }
}

} // namespace cparse
