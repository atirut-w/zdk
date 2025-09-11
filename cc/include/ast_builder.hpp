#pragma once
#include <CBaseVisitor.h>
#include <CParser.h>
#include <any>
#include <ast.hpp>
#include <memory>

struct ASTBuilder : public CBaseVisitor {
  std::any visitProgram(CParser::ProgramContext *ctx) override;
  std::any visitFunction(CParser::FunctionContext *ctx) override;
  std::any visitReturnStatement(CParser::ReturnStatementContext *ctx) override;
  std::any visitIntegerExpr(CParser::IntegerExprContext *ctx) override;
};
