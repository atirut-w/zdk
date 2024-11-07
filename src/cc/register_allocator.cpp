#include "register_allocator.hpp"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
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

  // First pass; put operand values in registers
  int used_registers = 0;
  std::vector<Value *> anywhere;
  for (int ninst = instructions.size() - 1; ninst >= 0; ninst--) {
    auto *instruction = instructions[ninst];

    // True means we found the start of a value's lifetime
    if (allocation.count(instruction)) {
      register_state &= ~allocation[instruction];
    }

    switch (instruction->getOpcode()) {
    case Instruction::Ret: {
      auto *ret = cast<ReturnInst>(instruction);
      if (auto *value = ret->getReturnValue()) {
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
      }
      break;
    }
    case Instruction::Add:
    case Instruction::Sub:
      auto *add = cast<BinaryOperator>(instruction);
      auto *lhs = add->getOperand(0);
      auto *rhs = add->getOperand(1);
      TypeSize size = module.getDataLayout().getTypeAllocSize(lhs->getType());

      switch (size) {
      case 1:
        allocation[lhs] = allocate_reg(R8_A);
        break;
      case 2:
        allocation[lhs] = allocate_reg(R16_HL);
        break;
      }

      anywhere.push_back(rhs);
      break;
    }
    used_registers |= register_state;
  }
  register_state = used_registers;

  // Second pass; allocate anything else
  for (int ninst = instructions.size() - 1; ninst >= 0; ninst--) {
    auto *instruction = instructions[ninst];

    for (int i = 0; i < instruction->getNumOperands(); i++) {
      auto *operand = instruction->getOperand(i);

      if (isa<AllocaInst>(operand)) {
        continue;
      }

      if (find(anywhere.begin(), anywhere.end(), operand) != anywhere.end()) {
        allocation[operand] = allocate(operand);
      }
    }
  }
}
