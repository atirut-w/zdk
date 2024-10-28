#include "codegen.hpp"
#include "CParser.h"
// #include "zir/instruction.hpp"
#include <any>
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
