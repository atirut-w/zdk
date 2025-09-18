#pragma once
#include <any>
#include <cparse/visitor.hpp>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

struct IREmitter : public cparse::Visitor<std::any> {
  llvm::Module &module;
  llvm::IRBuilder<> builder;

  IREmitter(llvm::LLVMContext &context, llvm::Module &module)
      : module(module), builder(context) {}
};
