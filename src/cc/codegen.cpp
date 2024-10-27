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
