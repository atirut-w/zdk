#pragma once
#include <llvm/IR/Instruction.h>

struct Allocation {
  llvm::Instruction *start;
  llvm::Instruction *end;
  bool spilled;
  int reg;
};
