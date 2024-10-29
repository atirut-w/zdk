#include "codegen.hpp"
#include "CParser.h"
#include "error.hpp"
// #include "zir/instruction.hpp"
#include <any>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Value.h>

using namespace std;
using namespace antlr4;
using namespace llvm;

std::any
Codegen::visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) {
  string name =
      dynamic_cast<CParser::FunctionDeclaratorContext *>(ctx->declarator())
          ->Identifier()
          ->getText();

  FunctionType *function_type =
      FunctionType::get(Type::getInt16Ty(module.getContext()), false);
  current_function =
      Function::Create(function_type, Function::ExternalLinkage, name, &module);
  current_block = BasicBlock::Create(module.getContext(), "", current_function);
  builder.SetInsertPoint(current_block);
  variables.clear();

  return visitChildren(ctx);
}

std::any Codegen::visitReturnStatement(CParser::ReturnStatementContext *ctx) {
  if (ctx->expression()) {
    Value *value = any_cast<Value *>(visit(ctx->expression()));
    builder.CreateRet(value);
  } else {
    builder.CreateRetVoid();
  }
  return {};
}

std::any
Codegen::visitDeclarationWithInit(CParser::DeclarationWithInitContext *ctx) {
  for (auto *declarator : ctx->initDeclarator()) {
    string name = declarator->declarator()->getText();
    Value *value = any_cast<Value *>(visit(declarator->initializer()));

    variables[name] = {value->getType(),
                       builder.CreateAlloca(value->getType())};
    builder.CreateStore(value, variables[name].alloca);
  }

  return {};
}

std::any Codegen::visitDeclarationWithoutInit(
    CParser::DeclarationWithoutInitContext *ctx) {
  string name = ctx->specifier().back()->getText();
  variables[name] = {
      Type::getInt16Ty(module.getContext()),
      builder.CreateAlloca(Type::getInt16Ty(module.getContext()))};
  return {};
}

std::any
Codegen::visitIdentifierExpression(CParser::IdentifierExpressionContext *ctx) {
  string name = ctx->Identifier()->getText();
  if (!variables.count(name)) {
    throw SemanticError(ctx, "use of undeclared identifier '" + name + "'");
  }

  return static_cast<Value *>(
      builder.CreateLoad(variables[name].type, variables[name].alloca));
}

std::any Codegen::visitIntegerConstantExpression(
    CParser::IntegerConstantExpressionContext *ctx) {
  return static_cast<Value *>(ConstantInt::get(
      Type::getInt16Ty(module.getContext()), stoi(ctx->getText())));
}

std::any Codegen::visitParenthesizedExpression(
    CParser::ParenthesizedExpressionContext *ctx) {
  return visit(ctx->expression());
}

std::any
Codegen::visitNegationExpression(CParser::NegationExpressionContext *ctx) {
  Value *value = any_cast<Value *>(visit(ctx->expression()));
  return builder.CreateNeg(value);
}

std::any
Codegen::visitBitwiseNotExpression(CParser::BitwiseNotExpressionContext *ctx) {
  Value *value = any_cast<Value *>(visit(ctx->expression()));
  return builder.CreateNot(value);
}

std::any Codegen::visitMultiplicativeExpression(
    CParser::MultiplicativeExpressionContext *ctx) {
  Value *lhs = any_cast<Value *>(visit(ctx->expression(0)));
  Value *rhs = any_cast<Value *>(visit(ctx->expression(1)));

  if (ctx->Multiply()) {
    return builder.CreateMul(lhs, rhs);
  } else if (ctx->Divide()) {
    return builder.CreateSDiv(lhs, rhs);
  } else if (ctx->Modulo()) {
    return builder.CreateSRem(lhs, rhs);
  }

  throw runtime_error("unknown operator");
}

std::any
Codegen::visitAdditiveExpression(CParser::AdditiveExpressionContext *ctx) {
  Value *lhs = any_cast<Value *>(visit(ctx->expression(0)));
  Value *rhs = any_cast<Value *>(visit(ctx->expression(1)));

  if (ctx->Plus()) {
    return builder.CreateAdd(lhs, rhs);
  } else if (ctx->Minus()) {
    return builder.CreateSub(lhs, rhs);
  }

  throw runtime_error("unknown operator");
}

std::any
Codegen::visitRelationalExpression(CParser::RelationalExpressionContext *ctx) {
  Value *lhs = any_cast<Value *>(visit(ctx->expression(0)));
  Value *rhs = any_cast<Value *>(visit(ctx->expression(1)));

  Value *cmp = nullptr;
  if (ctx->Less()) {
    cmp = builder.CreateICmpSLT(lhs, rhs);
  } else if (ctx->LessEqual()) {
    cmp = builder.CreateICmpSLE(lhs, rhs);
  } else if (ctx->Greater()) {
    cmp = builder.CreateICmpSGT(lhs, rhs);
  } else if (ctx->GreaterEqual()) {
    cmp = builder.CreateICmpSGE(lhs, rhs);
  }

  return builder.CreateZExt(cmp, Type::getInt16Ty(module.getContext()));
}

std::any
Codegen::visitEqualityExpression(CParser::EqualityExpressionContext *ctx) {
  Value *lhs = any_cast<Value *>(visit(ctx->expression(0)));
  Value *rhs = any_cast<Value *>(visit(ctx->expression(1)));

  Value *cmp = nullptr;
  if (ctx->Equal()) {
    cmp = builder.CreateICmpEQ(lhs, rhs);
  } else if (ctx->NotEqual()) {
    cmp = builder.CreateICmpNE(lhs, rhs);
  }

  return builder.CreateZExt(cmp, Type::getInt16Ty(module.getContext()));
}

std::any
Codegen::visitLogicalAndExpression(CParser::LogicalAndExpressionContext *ctx) {
  BasicBlock *rhs_block =
      BasicBlock::Create(module.getContext(), "", current_function);
  BasicBlock *end_block =
      BasicBlock::Create(module.getContext(), "", current_function);

  Value *lhs = any_cast<Value *>(visit(ctx->expression(0)));
  auto *cmp = builder.CreateICmpNE(
      lhs, ConstantInt::get(Type::getInt16Ty(module.getContext()), 0));
  builder.CreateCondBr(cmp, rhs_block, end_block);

  builder.SetInsertPoint(rhs_block);
  Value *rhs = any_cast<Value *>(visit(ctx->expression(1)));
  cmp = builder.CreateICmpNE(
      rhs, ConstantInt::get(Type::getInt16Ty(module.getContext()), 0));
  builder.CreateBr(end_block);

  builder.SetInsertPoint(end_block);
  PHINode *phi = builder.CreatePHI(Type::getInt16Ty(module.getContext()), 2);
  phi->addIncoming(ConstantInt::get(Type::getInt16Ty(module.getContext()), 0),
                   current_block);
  phi->addIncoming(rhs, rhs_block);

  return static_cast<Value *>(phi);
}

std::any
Codegen::visitLogicalOrExpression(CParser::LogicalOrExpressionContext *ctx) {
  BasicBlock *rhs_block =
      BasicBlock::Create(module.getContext(), "", current_function);
  BasicBlock *end_block =
      BasicBlock::Create(module.getContext(), "", current_function);

  Value *lhs = any_cast<Value *>(visit(ctx->expression(0)));
  auto *cmp = builder.CreateICmpNE(
      lhs, ConstantInt::get(Type::getInt16Ty(module.getContext()), 0));
  builder.CreateCondBr(cmp, end_block, rhs_block);

  builder.SetInsertPoint(rhs_block);
  Value *rhs = any_cast<Value *>(visit(ctx->expression(1)));
  cmp = builder.CreateICmpNE(
      rhs, ConstantInt::get(Type::getInt16Ty(module.getContext()), 0));
  builder.CreateBr(end_block);

  builder.SetInsertPoint(end_block);
  PHINode *phi = builder.CreatePHI(Type::getInt16Ty(module.getContext()), 2);
  phi->addIncoming(ConstantInt::get(Type::getInt16Ty(module.getContext()), 1),
                   current_block);
  phi->addIncoming(rhs, rhs_block);

  return static_cast<Value *>(phi);
}
