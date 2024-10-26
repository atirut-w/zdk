#pragma once
#include "CBaseVisitor.h"
#include "zir/module.hpp"
#include <string>

class Codegen : public CBaseVisitor {
  ZIR::Module module;
  // std::ostream &os;

  struct {
    ZIR::Module::Function *current_function;
    int temp = 0;
  } ctx;

  // int reserve_label() { return ctx.label++; }
  // std::string label(int n) { return std::to_string(n) + ":"; }
  // std::string forward_label(int n) { return std::to_string(n) + "f"; }
  std::string make_temp() { return "tmp." + std::to_string(ctx.temp++); }

  // virtual std::any visitTranslationUnit(CParser::TranslationUnitContext *ctx) override;
  virtual std::any visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;
  // virtual std::any visitReturnStatement(CParser::ReturnStatementContext *ctx) override;

  // Expressions in order of precedence
  virtual std::any visitParenthesizedExpression(CParser::ParenthesizedExpressionContext *ctx) override;
  virtual std::any visitIntegerConstantExpression(CParser::IntegerConstantExpressionContext *ctx) override;
  virtual std::any visitNegationExpression(CParser::NegationExpressionContext *ctx) override;
  virtual std::any visitBitwiseNotExpression(CParser::BitwiseNotExpressionContext *ctx) override;
  // virtual std::any visitMultiplicativeExpression(CParser::MultiplicativeExpressionContext *ctx) override;
  // virtual std::any visitAdditiveExpression(CParser::AdditiveExpressionContext *ctx) override;
  // virtual std::any visitLogicalAndExpression(CParser::LogicalAndExpressionContext *ctx) override;
  // virtual std::any visitLogicalOrExpression(CParser::LogicalOrExpressionContext *ctx) override;

public:
  // Codegen(std::ostream &os) : os(os) {}

  ZIR::Module &get_module() { return module; }
};
