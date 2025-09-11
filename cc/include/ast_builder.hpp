#pragma once
#include "CBaseVisitor.h"

class ASTBuilder : public CBaseVisitor {
public:
  virtual std::any visitTranslationUnit(CParser::TranslationUnitContext *ctx) override;
  virtual std::any visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;
  virtual std::any visitGlobalDeclarationWithoutInit(CParser::GlobalDeclarationWithoutInitContext *ctx) override;

  virtual std::any visitReturnStatement(CParser::ReturnStatementContext *ctx) override;
  virtual std::any visitExpressionStatement(CParser::ExpressionStatementContext *ctx) override;
  virtual std::any visitIfStatement(CParser::IfStatementContext *ctx) override;
  virtual std::any visitIfElseStatement(CParser::IfElseStatementContext *ctx) override;
  virtual std::any visitWhileStatement(CParser::WhileStatementContext *ctx) override;
  virtual std::any visitForStatement(CParser::ForStatementContext *ctx) override;
  virtual std::any visitNullStatement(CParser::NullStatementContext *ctx) override;

  virtual std::any visitIdentifierExpression(CParser::IdentifierExpressionContext *ctx) override;
  virtual std::any visitIntegerConstantExpression(CParser::IntegerConstantExpressionContext *ctx) override;
  virtual std::any visitParenthesizedExpression(CParser::ParenthesizedExpressionContext *ctx) override;
  virtual std::any visitMultiplicativeExpression(CParser::MultiplicativeExpressionContext *ctx) override;
  virtual std::any visitAdditiveExpression(CParser::AdditiveExpressionContext *ctx) override;
  virtual std::any visitRelationalExpression(CParser::RelationalExpressionContext *ctx) override;
  virtual std::any visitEqualityExpression(CParser::EqualityExpressionContext *ctx) override;
  virtual std::any visitAssignmentExpression(CParser::AssignmentExpressionContext *ctx) override;
};
