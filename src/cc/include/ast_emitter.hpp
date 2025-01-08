#pragma once
#include "CBaseVisitor.h"

class ASTEmitter : public CBaseVisitor {
public:
  virtual std::any visitTranslationUnit(CParser::TranslationUnitContext *ctx) override;
  virtual std::any visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;
  virtual std::any visitReturnStatement(CParser::ReturnStatementContext *ctx) override;

  virtual std::any visitIntegerConstantExpression(CParser::IntegerConstantExpressionContext *ctx) override;
};
