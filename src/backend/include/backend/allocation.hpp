#pragma once
#include <llvm/IR/Instruction.h>

struct Allocation {
  llvm::Instruction *start;
  llvm::Instruction *end;
  int reg;
};
