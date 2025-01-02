#pragma once
#include "backend/allocation.hpp"
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <map>

class Allocator {
  llvm::Module &module;
  int register_state = 0;

  int allocate(int size);
  int allocate_for(const llvm::Value *value);
  int free(int reg);

public:
  enum R8 {
    R8_A = 1 << 6,
    R8_B = 1 << 5,
    R8_C = 1 << 4,
    R8_D = 1 << 3,
    R8_E = 1 << 2,
    R8_H = 1 << 1,
    R8_L = 1 << 0,
  };

  enum R16 {
    R16_BC = R8_B | R8_C,
    R16_DE = R8_D | R8_E,
    R16_HL = R8_H | R8_L,
  };

  enum R32 {
    R32_BCDE = R16_BC | R16_DE,
    R32_DEHL = R16_DE | R16_HL,
  };

  Allocator(llvm::Module &module) : module(module) {}

  std::map<const llvm::Value *, Allocation> allocation;
  void run(llvm::Function &function);
};
