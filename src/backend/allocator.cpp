#include "include/backend/allocator.hpp"
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
    for (int reg : {R8_B, R8_C, R8_D, R8_E}) {
      if (register_state & reg) {
        continue;
      }
      register_state |= reg;
      return reg;
    }
    break;
  case 2:
    for (int reg : {R16_BC, R16_DE}) {
      if (register_state & reg) {
        continue;
      }
      register_state |= reg;
      return reg;
    }
    break;
  case 4:
    if (!(register_state & R32_BCDE)) {
      register_state |= R32_BCDE;
      return R32_BCDE;
    }
    break;
  }

  return 0;
}

Allocation Allocator::allocate_for(const Value *value) {
  Type *type = value->getType();
  int size = module.getDataLayout().getTypeAllocSize(type);

  if (isa<IntegerType>(type)) {
    int reg = allocate(size);
    if (reg) {
      return {false, reg};
    } else {
      return {true, 0};
    }
  } else {
    return {true, 0};
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

    // Check operands for for end of lifetime
    for (auto &operand : instruction->operands()) {
      if (isa<Instruction>(operand) && !isa<AllocaInst>(operand) && !isa<Constant>(operand)) {
        if (!allocation.count(operand)) {
          allocation[operand] = allocate_for(operand);
        }
      }
    }

    // Check instruction for start of lifetime
    if (!isa<AllocaInst>(instruction) && !instruction->getType()->isVoidTy()) {
      if (allocation.count(instruction)) {
        free(allocation[instruction].reg);
      } else {
        // Probably an unused instruction. Allocate then free right away
        allocation[instruction] = allocate_for(instruction);
        free(allocation[instruction].reg);
      }
    }
  }

  outs() << "Allocations:\n";
  for (auto &[value, alloc] : allocation) {
    outs() << *value << " -> " << alloc.reg;
    if (alloc.spilled) {
      outs() << " (spilled)";
    }
    outs() << "\n";
  }

  if (register_state & ~(R8_A | R16_HL)) {
    outs() << "Registers not freed: " << register_state << "\n";
  }
}
