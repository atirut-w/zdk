#include "ast_builder.hpp"

std::any ASTBuilder::visitProgram(CParser::ProgramContext *ctx) {
  auto program = new Program();
  program->function = std::unique_ptr<Function>(
      std::any_cast<Function *>(visit(ctx->function())));
  return program;
}

std::any ASTBuilder::visitFunction(CParser::FunctionContext *ctx) {
  auto func = new Function();
  func->name = ctx->Identifier()->getText();
  func->body = std::unique_ptr<Statement>(
      std::any_cast<Statement *>(visit(ctx->statement())));
  return func;
}

std::any ASTBuilder::visitReturnStatement(CParser::ReturnStatementContext *ctx) {
  auto stmt = new ReturnStatement();
  stmt->expr = std::unique_ptr<Expression>(
      std::any_cast<Expression *>(visit(ctx->expression())));
  return static_cast<Statement *>(stmt);
}

std::any ASTBuilder::visitIntegerExpr(CParser::IntegerExprContext *ctx) {
  auto expr = new Integer();
  expr->value = std::stoi(ctx->Integer()->getText());
  return static_cast<Expression *>(expr);
}
