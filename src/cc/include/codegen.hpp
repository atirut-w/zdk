#pragma once
#include "CBaseVisitor.h"

class Codegen : public CBaseVisitor {
  std::ostream &os;

  virtual std::any visitTranslationUnit(CParser::TranslationUnitContext *ctx) override;
  virtual std::any visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;
  virtual std::any visitReturnStatement(CParser::ReturnStatementContext *ctx) override;

  // Expressions in order of precedence
  virtual std::any visitIntegerConstantExpression(CParser::IntegerConstantExpressionContext *ctx) override;
  virtual std::any visitNegationExpression(CParser::NegationExpressionContext *ctx) override;
  virtual std::any visitBitwiseNotExpression(CParser::BitwiseNotExpressionContext *ctx) override;

public:
  Codegen(std::ostream &os) : os(os) {}
};
