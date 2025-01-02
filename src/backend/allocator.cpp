#include "include/backend/allocator.hpp"
#include "backend/allocation.hpp"
#include <bitset>
#include <iostream>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/TypeSize.h>
#include <stdexcept>
#include <vector>

using namespace std;
using namespace llvm;

int Allocator::allocate(int size) {
  switch (size) {
  default:
    throw runtime_error("unsupported register size");
  case 1:
    for (int reg : {R8_A, R8_B, R8_C, R8_D, R8_E, R8_H, R8_L}) {
      if (register_state & reg) {
        continue;
      }
      register_state |= reg;
      return reg;
    }
    break;
  case 2:
    for (int reg : {R16_BC, R16_DE, R16_HL}) {
      if (register_state & reg) {
        continue;
      }
      register_state |= reg;
      return reg;
    }
    break;
  case 4:
    for (int reg : {R32_BCDE, R32_DEHL}) {
      if (register_state & reg) {
        continue;
      }
      register_state |= reg;
      return reg;
    }
    break;
  }

  return 0;
}

int Allocator::allocate_for(const Value *value) {
  Type *type = value->getType();
  int size = module.getDataLayout().getTypeAllocSize(type);

  if (isa<IntegerType>(type)) {
    int reg = allocate(size);
    return reg;
  } else {
    return 0;
  }
}

int Allocator::free(int reg) {
  register_state &= ~reg;
  return 0;
}

void Allocator::run(Function &function) {
  std::vector<BasicBlock *> blocks;
  for (auto &block : function) {
    blocks.push_back(&block);
  }

  std::vector<Instruction *> instructions;
  for (auto &block : blocks) {
    for (auto &instruction : *block) {
      instructions.push_back(&instruction);
    }
  }

  for (int ninst = instructions.size() - 1; ninst >= 0; ninst--) {
    auto *instruction = instructions[ninst];

    // Check operands for end of lifetime
    for (auto &operand : instruction->operands()) {
      if (auto *opinst = dyn_cast<Instruction>(operand)) {
        if (!isa<AllocaInst>(opinst) && !opinst->getType()->isVoidTy()) {
          if (!allocation.count(opinst)) {
            Allocation alloc;
            alloc.reg = allocate_for(opinst);
            alloc.start = opinst;
            alloc.end = instruction;

            allocation[opinst] = alloc;
          }
        }
      }
    }

    // Check instruction for start of lifetime
    if (!isa<AllocaInst>(instruction) && !instruction->getType()->isVoidTy()) {
      if (allocation.count(instruction)) {
        free(allocation[instruction].reg);
      } else {
        Allocation alloc;
        alloc.reg = allocate_for(instruction);
        alloc.start = alloc.end = instruction;

        allocation[instruction] = alloc;
        free(alloc.reg);
      }
    }
  }

  // outs() << "Allocations:\n";
  // for (auto &[value, alloc] : allocation) {
  //   outs() << *value << " -> 0b" << bitset<8>(alloc.reg).to_string();
  //   if (!alloc.reg) {
  //     outs() << " (spilled)";
  //   }
  //   outs() << "\n";
  // }

  // if (register_state) {
  //   outs() << "Registers not freed: 0b" << bitset<8>(register_state).to_string() << "\n";
  // }
}
