#pragma once
#include <any>
#include <llvm/IR/Module.h>

struct IRVisitor {
  virtual std::any visit_module(llvm::Module &module);
  virtual std::any visit_function(llvm::Function &function);
  virtual std::any visit_block(llvm::BasicBlock &block);
  virtual std::any visit_instruction(llvm::Instruction &inst);
};
