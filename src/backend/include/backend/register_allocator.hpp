#pragma once
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <map>

class RegisterAllocator {
  llvm::Module &module;
  int register_state = 0;

  int allocate_reg(int reg);
  int allocate(const llvm::Value *value);
  int get_value_size(const llvm::Value *value);
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

  RegisterAllocator(llvm::Module &module) : module(module) {}

  std::map<const llvm::Value *, int> allocation;
  void run(llvm::Function &function);
};
