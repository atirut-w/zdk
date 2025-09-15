#pragma once
#include "ast.hpp"
#include "lexer.hpp"

namespace cparse {

class Parser {
  Lexer &lexer;
  
  Token expect(Token::Kind kind);

public:
  Parser(Lexer &lexer) : lexer(lexer) {}

  std::unique_ptr<TranslationUnit> translation_unit();
  std::unique_ptr<FunctionDefinition> function_definition();
  std::unique_ptr<Statement> statement();
  std::unique_ptr<ReturnStatement> return_statement();
  std::unique_ptr<Expression> expression();

  UnaryExpression::Operator parse_unary_operator();
};

}
