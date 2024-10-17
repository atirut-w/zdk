#pragma once
#include "CBaseVisitor.h"

class Codegen : public CBaseVisitor {
  std::ostream &os;

  virtual std::any visitTranslationUnit(CParser::TranslationUnitContext *ctx) override;
  virtual std::any visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;
  virtual std::any visitReturnStatement(CParser::ReturnStatementContext *ctx) override;
  virtual std::any visitExpression(CParser::ExpressionContext *ctx) override;

public:
  Codegen(std::ostream &os) : os(os) {}
};
