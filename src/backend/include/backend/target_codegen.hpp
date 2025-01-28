#pragma once
#include "backend/ralloc.hpp"
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <ostream>

class TargetCodegen {
protected:
  std::ostream &os;
  llvm::Module &module;

  virtual AllocationMap allocate_registers(llvm::Function &func) = 0;

public:
  TargetCodegen(std::ostream &os, llvm::Module &module) : os(os), module(module) {}
  virtual ~TargetCodegen() = default;
  
  virtual void emit() = 0;
};
