#pragma once
#include <any>
#include <llvm/IR/Module.h>

struct IRVisitor {
  virtual void visit(llvm::Module &module);
  virtual void visit(llvm::Function &function);
  virtual void visit(llvm::BasicBlock &block);
  virtual void visit(llvm::Instruction &inst);
};
