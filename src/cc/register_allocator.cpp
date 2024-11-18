#include "register_allocator.hpp"
#include <iostream>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/TypeSize.h>
#include <vector>

using namespace std;
using namespace llvm;

int RegisterAllocator::allocate_reg(int reg) {
  if (register_state & reg) {
    throw runtime_error("could not allocate register");
  }
  register_state |= reg;
  return reg;
}

int RegisterAllocator::allocate(const Value *value) {
  Type *type = value->getType();
  TypeSize size = module.getDataLayout().getTypeAllocSize(type);
  if (isa<AllocaInst>(value)) {
    return 0;
  }

  int register_mask = (1 << size) - 1;

  switch (size) {
  default:
    throw runtime_error("unsupported register size");
  case 1:
    for (int reg = 0; reg < 7; reg++) {
      if (register_state & (register_mask << reg)) {
        continue;
      }
      return allocate_reg(1 << reg);
    }
    break;
  case 2:
    for (int pair = 0; pair < 3; pair++) {
      if (register_state & (register_mask << (pair * 2))) {
        continue;
      }
      return allocate_reg(3 << (pair * 2));
    }
    break;
  case 4:
    for (int quad = 0; quad < 2; quad++) {
      if (register_state & (register_mask << (quad * 2))) {
        continue;
      }
      return allocate_reg(15 << (quad * 2));
    }
    break;
  }

  throw runtime_error("could not allocate register");
}

int RegisterAllocator::get_value_size(const Value *value) {
  if (auto *alloca = dyn_cast<AllocaInst>(value)) {
    return module.getDataLayout().getTypeAllocSize(alloca->getAllocatedType());
  } else {
    return module.getDataLayout().getTypeAllocSize(value->getType());
  }
}

void RegisterAllocator::run(Function &function) {
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

    if (allocation.count(instruction)) {
      register_state &= ~allocation[instruction];
    }

    switch (instruction->getOpcode()) {
    default:
      for (auto &operand : instruction->operands()) {
        if (auto *value = dyn_cast<Value>(&operand)) {
          if (isa<AllocaInst>(value)) {
            continue;
          }
          if (allocation.count(value)) {
            outs() << "WARN: " << *value << " is already allocated\n";
            continue;
          }

          allocation[value] = allocate(value);

          if (isa<Constant>(value)) {
            register_state &= ~allocation[value];
          }
        }
      }
      break;

    case Instruction::Ret: {
      if (auto *value = instruction->getOperand(0)) {
        switch (get_value_size(value)) {
        case 1:
          allocation[value] = allocate_reg(R8_A);
          break;
        case 2:
          allocation[value] = allocate_reg(R16_HL);
          break;
        case 4:
          allocation[value] = allocate_reg(R32_DEHL);
          break;
        }

        if (isa<Constant>(value)) {
          register_state &= ~allocation[value];
        }
      }
      break;
    }

    case Instruction::Add:
    case Instruction::Sub:
    case Instruction::ICmp:
    case Instruction::Xor: {
      Value *lhs = instruction->getOperand(0);
      Value *rhs = instruction->getOperand(1);
      int size = get_value_size(lhs);

      switch (size) {
      case 1:
        allocation[lhs] = allocate_reg(R8_A);
        break;
      case 2:
        allocation[lhs] = allocate_reg(R16_HL);
        break;
      case 4:
        allocation[lhs] = allocate_reg(R32_DEHL);
        break;
      }

      allocation[rhs] = allocate(rhs);

      if (isa<Constant>(lhs)) {
        register_state &= ~allocation[rhs];
      }
      if (isa<Constant>(rhs)) {
        register_state &= ~allocation[rhs];
      }
      break;
    }

    case Instruction::Br: {
      auto *branch = cast<BranchInst>(instruction);
      if (branch->isConditional()) {
        Value *condition = branch->getCondition();
        allocation[condition] = allocate_reg(R8_A);

        if (isa<Constant>(condition)) {
          register_state &= ~allocation[condition];
        }
      }
      break;
    }

    case Instruction::PHI: {
      auto *phi = cast<PHINode>(instruction);
      for (int i = 0; i < phi->getNumIncomingValues(); i++) {
        Value *value = phi->getIncomingValue(i);
        allocation[value] = allocation[phi];

        if (isa<Constant>(value)) {
          register_state &= ~allocation[value];
        }
      }
      break;
    }

    case Instruction::ZExt: {
      Value *operand = instruction->getOperand(0);

      switch (get_value_size(operand)) {
      case 1:
        allocation[operand] = allocate_reg(R8_L);
        break;
      case 2:
        allocation[operand] = allocate_reg(R16_HL);
        break;
      case 4:
        allocation[operand] = allocate_reg(R32_DEHL);
        break;
      }

      if (isa<Constant>(operand)) {
        register_state &= ~allocation[operand];
      }
      break;
    }
    }
  }
}
