#include "ast_builder.hpp"
#include "ast.hpp"
#include <memory>

using namespace std;

any ASTBuilder::visitTranslationUnit(CParser::TranslationUnitContext *ctx) {
  auto tu = new TranslationUnit();

  for (auto decl : ctx->declaration()) {
    tu->declarations.push_back(unique_ptr<ExternalDeclaration>(any_cast<ExternalDeclaration *>(visit(decl))));
  }

  return tu;
}

any ASTBuilder::visitVariableDeclaration(CParser::VariableDeclarationContext *ctx) {
  auto vd = new VariableDeclaration();
  vd->name = ctx->Identifier()->getText();
  
  if (!ctx->specifier().empty()) {
    vd->type = ctx->specifier(0)->getText();
  }
  
  if (ctx->expression()) {
    vd->initializer = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression())));
  }

  return static_cast<ExternalDeclaration *>(vd);
}

any ASTBuilder::visitFunctionDeclaration(CParser::FunctionDeclarationContext *ctx) {
  auto fd = new FunctionDeclaration();
  fd->name = ctx->Identifier()->getText();
  
  if (!ctx->specifier().empty()) {
    fd->return_type = ctx->specifier(0)->getText();
  }

  if (ctx->block()) {
    // Process block items directly into the function body
    for (auto item : ctx->block()->blockItem()) {
      auto result = visit(item);
      if (auto *stmt = any_cast<Statement *>(&result)) {
        fd->body.push_back(unique_ptr<ASTNode>(static_cast<ASTNode *>(*stmt)));
      } else if (auto *decl = any_cast<ExternalDeclaration *>(&result)) {
        fd->body.push_back(unique_ptr<ASTNode>(static_cast<ASTNode *>(*decl)));
      }
    }
  }

  return static_cast<ExternalDeclaration *>(fd);
}

any ASTBuilder::visitReturnStatement(CParser::ReturnStatementContext *ctx) {
  auto rs = new ReturnStatement();

  if (ctx->expression()) {
    rs->expression = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression())));
  }

  return static_cast<Statement *>(rs);
}

any ASTBuilder::visitExpressionStatement(CParser::ExpressionStatementContext *ctx) {
  auto es = new ExpressionStatement();
  es->expression = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression())));

  return static_cast<Statement *>(es);
}

any ASTBuilder::visitIfStatement(CParser::IfStatementContext *ctx) {
  auto is = new IfStatement();

  is->condition = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression())));
  is->then_statement = unique_ptr<Statement>(any_cast<Statement *>(visit(ctx->statement(0))));
  
  // Handle optional else clause
  if (ctx->statement().size() > 1) {
    is->else_statement = unique_ptr<Statement>(any_cast<Statement *>(visit(ctx->statement(1))));
  }

  return static_cast<Statement *>(is);
}

any ASTBuilder::visitDoWhileStatement(CParser::DoWhileStatementContext *ctx) {
  auto dws = new DoWhileStatement();
  
  dws->body = unique_ptr<Statement>(any_cast<Statement *>(visit(ctx->statement())));
  dws->condition = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression())));

  return static_cast<Statement *>(dws);
}

any ASTBuilder::visitWhileStatement(CParser::WhileStatementContext *ctx) {
  auto ws = new WhileStatement();

  ws->condition = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression())));
  ws->body = unique_ptr<Statement>(any_cast<Statement *>(visit(ctx->statement())));

  return static_cast<Statement *>(ws);
}

any ASTBuilder::visitForStatement(CParser::ForStatementContext *ctx) {
  auto fs = new ForStatement();

  if (ctx->forInit()) {
    fs->init = unique_ptr<ASTNode>(any_cast<ASTNode *>(visit(ctx->forInit())));
  }
  
  if (ctx->expression().size() >= 1) {
    fs->condition = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression(0))));
  }
  if (ctx->expression().size() >= 2) {
    fs->update = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression(1))));
  }
  
  fs->body = unique_ptr<Statement>(any_cast<Statement *>(visit(ctx->statement())));

  return static_cast<Statement *>(fs);
}

any ASTBuilder::visitNullStatement(CParser::NullStatementContext *ctx) {
  return static_cast<Statement *>(0);
}

any ASTBuilder::visitIdentifierExpression(CParser::IdentifierExpressionContext *ctx) {
  auto id = new IdentifierExpression();
  id->name = ctx->Identifier()->getText();

  return static_cast<Expression *>(id);
}

any ASTBuilder::visitIntegerConstantExpression(CParser::IntegerConstantExpressionContext *ctx) {
  auto ic = new IntegerConstant();
  ic->value = stoi(ctx->getText());

  return static_cast<Expression *>(ic);
}

any ASTBuilder::visitParenthesizedExpression(CParser::ParenthesizedExpressionContext *ctx) {
  return visit(ctx->expression());
}

any ASTBuilder::visitMultiplicativeExpression(CParser::MultiplicativeExpressionContext *ctx) {
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

any ASTBuilder::visitAdditiveExpression(CParser::AdditiveExpressionContext *ctx) {
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

any ASTBuilder::visitRelationalExpression(CParser::RelationalExpressionContext *ctx) {
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

any ASTBuilder::visitEqualityExpression(CParser::EqualityExpressionContext *ctx) {
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

any ASTBuilder::visitAssignmentExpression(CParser::AssignmentExpressionContext *ctx) {
  auto as = new Assignment();

  as->lvalue = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression(0))));
  as->rvalue = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression(1))));

  return static_cast<Expression *>(as);
}
