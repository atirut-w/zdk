#pragma once
#include "CBaseVisitor.h"

class ASTEmitter : public CBaseVisitor {
public:
  virtual std::any visitTranslationUnit(CParser::TranslationUnitContext *ctx) override;
  virtual std::any visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;
  virtual std::any visitReturnStatement(CParser::ReturnStatementContext *ctx) override;
  virtual std::any visitExpressionStatement(CParser::ExpressionStatementContext *ctx) override;

  virtual std::any visitIntegerConstantExpression(CParser::IntegerConstantExpressionContext *ctx) override;
  virtual std::any visitParenthesizedExpression(CParser::ParenthesizedExpressionContext *ctx) override;
  virtual std::any visitMultiplicativeExpression(CParser::MultiplicativeExpressionContext *ctx) override;
  virtual std::any visitAdditiveExpression(CParser::AdditiveExpressionContext *ctx) override;
  virtual std::any visitRelationalExpression(CParser::RelationalExpressionContext *ctx) override;
  virtual std::any visitEqualityExpression(CParser::EqualityExpressionContext *ctx) override;
};
