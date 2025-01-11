#pragma once
#include "CBaseVisitor.h"
#include "ast.hpp"
#include <ostream>

class Codegen : public CBaseVisitor {
  std::ostream &os;

public:
  Codegen(std::ostream &os) : os(os) {}

  virtual void gen_expression(Expression *expr);

  virtual std::any visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;
  virtual std::any visitReturnStatement(CParser::ReturnStatementContext *ctx) override;

  virtual std::any visitIntegerConstantExpression(CParser::IntegerConstantExpressionContext *ctx) override;
  virtual std::any visitParenthesizedExpression(CParser::ParenthesizedExpressionContext *ctx) override;
  virtual std::any visitMultiplicativeExpression(CParser::MultiplicativeExpressionContext *ctx) override;
  virtual std::any visitAdditiveExpression(CParser::AdditiveExpressionContext *ctx) override;
};
