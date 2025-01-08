#pragma once
#include "CBaseVisitor.h"
#include <any>
#include <ostream>
#include <string>

struct ExpressionCtx {
  // std::string reg;
};

class Codegen : public CBaseVisitor {
  std::ostream &os;

  void load_imm(std::string reg, int value);

public:
  Codegen(std::ostream &os) : os(os) {}

  virtual std::any visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;
  virtual std::any visitReturnStatement(CParser::ReturnStatementContext *ctx) override;
  virtual std::any visitIntegerConstantExpression(CParser::IntegerConstantExpressionContext *ctx) override;
};
