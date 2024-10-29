#pragma once
#include "CBaseVisitor.h"
#include "llvm/IR/IRBuilder.h"
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <string>

struct Variable {
  llvm::Type *type;
  llvm::AllocaInst *alloca;
};

class Codegen : public CBaseVisitor {
  llvm::Module &module;

  llvm::Function *current_function = nullptr;
  llvm::BasicBlock *current_block = nullptr;
  llvm::IRBuilder<> builder;
  std::map<std::string, Variable> variables;

  virtual std::any
  visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;
  virtual std::any
  visitReturnStatement(CParser::ReturnStatementContext *ctx) override;

  virtual std::any
  visitDeclarationWithInit(CParser::DeclarationWithInitContext *ctx) override;
  virtual std::any visitDeclarationWithoutInit(
      CParser::DeclarationWithoutInitContext *ctx) override;

  // Expressions in order of precedence
  virtual std::any
  visitIdentifierExpression(CParser::IdentifierExpressionContext *ctx) override;
  virtual std::any visitIntegerConstantExpression(
      CParser::IntegerConstantExpressionContext *ctx) override;
  virtual std::any visitParenthesizedExpression(
      CParser::ParenthesizedExpressionContext *ctx) override;
  virtual std::any
  visitNegationExpression(CParser::NegationExpressionContext *ctx) override;
  virtual std::any
  visitBitwiseNotExpression(CParser::BitwiseNotExpressionContext *ctx) override;
  virtual std::any visitMultiplicativeExpression(
      CParser::MultiplicativeExpressionContext *ctx) override;
  virtual std::any
  visitAdditiveExpression(CParser::AdditiveExpressionContext *ctx) override;
  virtual std::any
  visitRelationalExpression(CParser::RelationalExpressionContext *ctx) override;
  virtual std::any
  visitEqualityExpression(CParser::EqualityExpressionContext *ctx) override;
  virtual std::any
  visitLogicalAndExpression(CParser::LogicalAndExpressionContext *ctx) override;
  virtual std::any
  visitLogicalOrExpression(CParser::LogicalOrExpressionContext *ctx) override;

public:
  Codegen(llvm::Module &module)
      : module(module), builder(module.getContext()) {}

  llvm::Module &get_module() { return module; }
};
