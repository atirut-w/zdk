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
  std::unique_ptr<Block> block();
  std::unique_ptr<Declaration> declaration();
  std::unique_ptr<Statement> statement();
  std::unique_ptr<CompoundStatement> compound_statement();
  std::unique_ptr<ReturnStatement> return_statement();
  std::unique_ptr<Statement> expression_statement();
  std::unique_ptr<IfStatement> if_statement();

  std::unique_ptr<Expression> factor();
  std::unique_ptr<Expression> expression(int min_prec = 0);
  std::unique_ptr<Expression> conditional_middle();
  UnaryExpression::Operator unary_operator();
  BinaryExpression::Operator binary_operator();
};

}
