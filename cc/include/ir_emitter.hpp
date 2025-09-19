#pragma once
#include <any>
#include <cparse/visitor.hpp>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <vector>

struct Symbol {
  std::string name;
  llvm::Value *value;
  llvm::Type *type;
};

class IREmitter : public cparse::Visitor<std::any> {
  llvm::LLVMContext &context;
  llvm::Module &module;
  llvm::IRBuilder<> builder;

  std::vector<Symbol> symbols;

  Symbol *getSymbol(const std::string &name);

public:
  IREmitter(llvm::LLVMContext &context, llvm::Module &module)
      : context(context), module(module), builder(context) {}

  using cparse::Visitor<std::any>::visit;

  std::any visit(cparse::FunctionDefinition &node) override;
  std::any visit(cparse::Block &node) override;
  std::any visit(cparse::Declaration &node) override;
  
  std::any visit(cparse::ReturnStatement &node) override;
  std::any visit(cparse::ExpressionStatement &node) override;
  std::any visit(cparse::IfStatement &node) override;
  std::any visit(cparse::CompoundStatement &node) override;

  std::any visit(cparse::ConstantExpression &node) override;
  std::any visit(cparse::IdentifierExpression &node) override;
  std::any visit(cparse::UnaryExpression &node) override;
  std::any visit(cparse::BinaryExpression &node) override;
  std::any visit(cparse::AssignmentExpression &node) override;
  std::any visit(cparse::ConditionalExpression &node) override;
};
