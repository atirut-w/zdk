#include "ast_emitter.hpp"
#include "ast.hpp"
#include <memory>

using namespace std;

any ASTEmitter::visitTranslationUnit(CParser::TranslationUnitContext *ctx) {
  auto tu = new TranslationUnit();

  for (auto decl : ctx->externalDeclaration()) {
    tu->declarations.push_back(unique_ptr<ExternalDeclaration>(any_cast<ExternalDeclaration *>(visit(decl))));
  }

  return tu;
}

any ASTEmitter::visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) {
  auto fd = new FunctionDefinition();
  fd->name = dynamic_cast<CParser::FunctionDeclaratorContext *>(ctx->declarator())->Identifier()->getText();

  for (auto stmt : ctx->statement()) {
    fd->body.push_back(unique_ptr<Statement>(any_cast<Statement *>(visit(stmt))));
  }

  return static_cast<ExternalDeclaration *>(fd);
}

any ASTEmitter::visitReturnStatement(CParser::ReturnStatementContext *ctx) {
  auto rs = new ReturnStatement();

  if (ctx->expression()) {
    rs->expression = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression())));
  }

  return static_cast<Statement *>(rs);
}

any ASTEmitter::visitIntegerConstantExpression(CParser::IntegerConstantExpressionContext *ctx) {
  auto ic = new IntegerConstant();
  ic->value = stoi(ctx->getText());

  return static_cast<Expression *>(ic);
}

any ASTEmitter::visitParenthesizedExpression(CParser::ParenthesizedExpressionContext *ctx) {
  return visit(ctx->expression());
}

any ASTEmitter::visitMultiplicativeExpression(CParser::MultiplicativeExpressionContext *ctx) {
  auto be = new BinaryExpression();

  be->left = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression(0))));
  be->right = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression(1))));

  if (ctx->Multiply()) {
    be->op = BinaryExpression::Operator::Mul;
  } else if (ctx->Divide()) {
    be->op = BinaryExpression::Operator::Div;
  } else if (ctx->Modulo()) {
    be->op = BinaryExpression::Operator::Mod;
  }

  return static_cast<Expression *>(be);
}

any ASTEmitter::visitAdditiveExpression(CParser::AdditiveExpressionContext *ctx) {
  auto be = new BinaryExpression();

  be->left = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression(0))));
  be->right = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression(1))));

  if (ctx->Add()) {
    be->op = BinaryExpression::Operator::Add;
  } else if (ctx->Subtract()) {
    be->op = BinaryExpression::Operator::Sub;
  }

  return static_cast<Expression *>(be);
}
