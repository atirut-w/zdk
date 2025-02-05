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

any ASTEmitter::visitGlobalDeclarationWithoutInit(CParser::GlobalDeclarationWithoutInitContext *ctx) {
  auto gd = new GlobalDeclaration();
  gd->name = ctx->specifier(ctx->specifier().size() - 1)->getText();

  return static_cast<ExternalDeclaration *>(gd);
}

any ASTEmitter::visitReturnStatement(CParser::ReturnStatementContext *ctx) {
  auto rs = new ReturnStatement();

  if (ctx->expression()) {
    rs->expression = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression())));
  }

  return static_cast<Statement *>(rs);
}

any ASTEmitter::visitExpressionStatement(CParser::ExpressionStatementContext *ctx) {
  auto es = new ExpressionStatement();
  es->expression = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression())));

  return static_cast<Statement *>(es);
}

any ASTEmitter::visitIfStatement(CParser::IfStatementContext *ctx) {
  auto is = new IfStatement();

  is->condition = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression())));
  is->then_statement = unique_ptr<Statement>(any_cast<Statement *>(visit(ctx->statement())));

  return static_cast<Statement *>(is);
}

any ASTEmitter::visitIfElseStatement(CParser::IfElseStatementContext *ctx) {
  auto is = new IfStatement();

  is->condition = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression())));
  is->then_statement = unique_ptr<Statement>(any_cast<Statement *>(visit(ctx->statement(0))));
  is->else_statement = unique_ptr<Statement>(any_cast<Statement *>(visit(ctx->statement(1))));

  return static_cast<Statement *>(is);
}

any ASTEmitter::visitWhileStatement(CParser::WhileStatementContext *ctx) {
  auto ws = new WhileStatement();

  ws->condition = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression())));
  ws->body = unique_ptr<Statement>(any_cast<Statement *>(visit(ctx->statement())));

  return static_cast<Statement *>(ws);
}

any ASTEmitter::visitIdentifierExpression(CParser::IdentifierExpressionContext *ctx) {
  auto id = new Identifier();
  id->name = ctx->Identifier()->getText();

  return static_cast<Expression *>(id);
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
    be->op = BinaryExpression::Mul;
  } else if (ctx->Divide()) {
    be->op = BinaryExpression::Div;
  } else if (ctx->Modulo()) {
    be->op = BinaryExpression::Mod;
  }

  return static_cast<Expression *>(be);
}

any ASTEmitter::visitAdditiveExpression(CParser::AdditiveExpressionContext *ctx) {
  auto be = new BinaryExpression();

  be->left = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression(0))));
  be->right = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression(1))));

  if (ctx->Add()) {
    be->op = BinaryExpression::Add;
  } else if (ctx->Subtract()) {
    be->op = BinaryExpression::Sub;
  }

  return static_cast<Expression *>(be);
}

any ASTEmitter::visitRelationalExpression(CParser::RelationalExpressionContext *ctx) {
  auto re = new RelationalExpression();

  re->left = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression(0))));
  re->right = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression(1))));

  if (ctx->Less()) {
    re->op = RelationalExpression::Lt;
  } else if (ctx->LessEqual()) {
    re->op = RelationalExpression::Le;
  } else if (ctx->Greater()) {
    re->op = RelationalExpression::Gt;
  } else if (ctx->GreaterEqual()) {
    re->op = RelationalExpression::Ge;
  }

  return static_cast<Expression *>(re);
}

any ASTEmitter::visitEqualityExpression(CParser::EqualityExpressionContext *ctx) {
  auto re = new RelationalExpression();

  re->left = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression(0))));
  re->right = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression(1))));

  if (ctx->Equal()) {
    re->op = RelationalExpression::Eq;
  } else if (ctx->NotEqual()) {
    re->op = RelationalExpression::Ne;
  }

  return static_cast<Expression *>(re);
}

any ASTEmitter::visitAssignmentExpression(CParser::AssignmentExpressionContext *ctx) {
  auto as = new Assignment();

  as->lvalue = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression(0))));
  as->rvalue = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression(1))));

  return static_cast<Expression *>(as);
}
