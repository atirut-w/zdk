#include "include/backend/asm_printer.hpp"
#include "include/backend/register_allocator.hpp"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/TypeSize.h>
#include <llvm/Support/raw_ostream.h>
#include <string>

using namespace std;
using namespace llvm;

map<int, string> register_names = {
    {RegisterAllocator::R8_A, "a"},        {RegisterAllocator::R8_B, "b"},        {RegisterAllocator::R8_C, "c"},    {RegisterAllocator::R8_D, "d"},
    {RegisterAllocator::R8_E, "e"},        {RegisterAllocator::R8_H, "h"},        {RegisterAllocator::R8_L, "l"},

    {RegisterAllocator::R16_BC, "bc"},     {RegisterAllocator::R16_DE, "de"},     {RegisterAllocator::R16_HL, "hl"},

    {RegisterAllocator::R32_BCDE, "bcde"}, {RegisterAllocator::R32_DEHL, "dehl"},
};

void AsmPrinter::generate_prologue() {
  int offset = 0;
  int blocknum = 0;
  for (auto &block : *current_function) {
    blocknums[&block] = blocknum++;
    for (auto &instruction : block) {
      if (auto alloca = dyn_cast<AllocaInst>(&instruction)) {
        Type *type = alloca->getAllocatedType();
        TypeSize size = module.getDataLayout().getTypeAllocSize(type);
        offset -= size;
        offsets[&instruction] = offset + 1;
      }
    }
  }

  if (offset < 0) {
    os << "\tpush ix\n";
    os << "\tld ix, 0\n";
    os << "\tadd ix, sp\n";

    os << "\tld hl, " << offset << "\n";
    os << "\tadd hl, sp\n";
    os << "\tld sp, hl\n";
  }
}

string AsmPrinter::get_ix(int base, int offset) {
  if (base + offset <= 0) {
    return "(ix-" + to_string(-(base + offset)) + ")";
  } else {
    return "(ix+" + to_string(base + offset) + ")";
  }
}

string AsmPrinter::get_label(const BasicBlock *block) {
  string label = to_string(blocknums[block]);
  if (blocknums[block] < blocknums[current_block]) {
    label += "b";
  } else {
    label += "f";
  }
  return label;
}

string AsmPrinter::get_register_of(const Value *value) {
  return register_names[allocation[value]];
}

void AsmPrinter::check_phi(const BasicBlock *block) {
  if (auto *phi = dyn_cast<PHINode>(block->begin())) {
    for (unsigned i = 0; i < phi->getNumIncomingValues(); i++) {
      BasicBlock *incoming_block = phi->getIncomingBlock(i);
      Value *value = phi->getIncomingValue(i);

      if (incoming_block == current_block) {
        load_value(value, allocation[phi]);
      }
    }
  }
}

int AsmPrinter::load_value(const Value *value, int reg, bool sign_extend) {
  reg = reg ? reg : allocation[value];
  string reg_name = register_names[reg];

  // cout << "loading value into " << reg_name << "\n";
  if (auto *constant = dyn_cast<ConstantInt>(value)) {
    Type *type = value->getType();
    TypeSize size = module.getDataLayout().getTypeAllocSize(type);
    int value = sign_extend ? constant->getSExtValue() : constant->getZExtValue();

    if (size < 4) {
      os << "\tld " << reg_name << ", " << value << "\n";
    } else {
      os << "\tld " << reg_name.substr(2, 2) << ", " << (value & 0xff) << "\n";
      os << "\tld " << reg_name.substr(0, 2) << ", " << (value >> 8) << "\n";
    }

    return size;
  } else if (auto *alloca = dyn_cast<AllocaInst>(value)) {
    int offset = offsets[value];
    Type *type = alloca->getAllocatedType();
    TypeSize size = module.getDataLayout().getTypeAllocSize(type);

    for (int i = 0; i < size; i++) {
      os << "\tld " << reg_name[size - i - 1] << ", " << get_ix(offset, i) << "\n";
    }

    return size;
  }

  return module.getDataLayout().getTypeAllocSize(value->getType());
}

void AsmPrinter::copy(int from, int to) {
  string from_reg = register_names[from];
  string to_reg = register_names[to];

  if (from_reg == to_reg) {
    return;
  }

  for (int i = 0; i < min(from_reg.length(), to_reg.length()); i++) {
    if (from_reg[from_reg.length() - i - 1] == to_reg[to_reg.length() - i - 1]) {
      continue;
    }
    os << "\tld " << to_reg[to_reg.length() - i - 1] << ", " << from_reg[from_reg.length() - i - 1] << "\n";
  }
}

void AsmPrinter::print() {
  os << "\t; Generated by ZDK C Compiler\n\n";

  for (auto &function : module) {
    string name = function.getName().str();
    current_function = &function;

    RegisterAllocator allocator(module);
    allocator.run(function);
    allocation = allocator.allocation;

    os << "\t.global " << name << "\n";
    os << name << ":\n";
    generate_prologue();

    int ninst = 0;
    for (auto &block : function) {
      current_block = &block;
      os << blocknums[&block] << ":\n";
      for (auto &instruction : block) {
        string comment = "; ";
        raw_string_ostream rso(comment);
        instruction.print(rso);
        os << comment << "\n";

        // Ignore unused instructions
        if (!instruction.getType()->isVoidTy() && instruction.getNumUses() == 0) {
          os << "\t; (UNUSED)\n";
          continue;
        }
        
        print_instruction(&instruction);
        ninst++;
      }
    }
  }
}

void AsmPrinter::print_instruction(const Instruction *instruction) {
  switch (instruction->getOpcode()) {
  default:
    outs() << *instruction << "\n";
    // throw runtime_error("unhandled instruction");
    cerr << "unhandled opcode: " << instruction->getOpcode() << "\n";
    break;

  // Terminator instructions :robot:
  case Instruction::Ret:
    print_return(cast<ReturnInst>(instruction));
    break;
  case Instruction::Br:
    print_br(cast<BranchInst>(instruction));
    break;

  // Binary instructions
  case Instruction::Add:
    print_add(cast<BinaryOperator>(instruction));
    break;
  case Instruction::Sub:
    print_sub(cast<BinaryOperator>(instruction));
    break;
  case Instruction::Xor:
    print_xor(cast<BinaryOperator>(instruction));
    break;

  // Memory instructions
  case Instruction::Alloca:
    break;
  case Instruction::Load:
    print_load(cast<LoadInst>(instruction));
    break;
  case Instruction::Store:
    print_store(cast<StoreInst>(instruction));
    break;

  // Cast instructions
  case Instruction::ZExt:
    print_zext(cast<ZExtInst>(instruction));
    break;

  // Other instructions
  case Instruction::ICmp:
    print_icmp(cast<ICmpInst>(instruction));
    break;
  case Instruction::PHI:
    break;
  }
}

void AsmPrinter::print_return(const ReturnInst *ret) {
  if (auto *value = ret->getReturnValue()) {
    load_value(value);
  }
  if (!offsets.empty()) {
    os << "\tld sp, ix\n";
    os << "\tpop ix\n";
  }
  os << "\tret\n";
}

void AsmPrinter::print_br(const BranchInst *br) {
  if (br->isConditional()) {
    BasicBlock *true_block = br->getSuccessor(0);
    BasicBlock *false_block = br->getSuccessor(1);

    load_value(br->getCondition());
    string reg = get_register_of(br->getCondition());
    copy(allocation[br->getCondition()], RegisterAllocator::R8_A);
    
    if (reg.length() == 1) {
      os << "\tor a\n";
    } else {
      for (int i = 1; i < reg.length(); i++) {
        os << "\tor " << reg[i] << "\n";
      }
    }

    check_phi(true_block);
    os << "\tjr nz, " << get_label(true_block) << "\n";
    check_phi(false_block);
    os << "\tjr " << get_label(false_block) << "\n";
  } else {
    check_phi(br->getSuccessor(0));
    os << "\tjr " << get_label(br->getSuccessor(0)) << "\n";
  }
}

void AsmPrinter::print_add(const BinaryOperator *add) {
  Value *lhs = add->getOperand(0);
  Value *rhs = add->getOperand(1);
  int size = load_value(lhs);
  
  switch (size) {
  case 1:
  case 2:
    load_value(rhs);
    os << "\tadd " << get_register_of(lhs) << ", " << get_register_of(rhs) << "\n";
    break;
  case 4:
    os << "\tpush " << get_register_of(lhs).substr(2, 2) << "\n";
    os << "\tpush " << get_register_of(lhs).substr(0, 2) << "\n";
    load_value(rhs);
    os << "\tpush " << get_register_of(rhs).substr(2, 2) << "\n";
    os << "\tpush " << get_register_of(rhs).substr(0, 2) << "\n";
    os << "\tcall __adddi3\n";
    os << "\tpop bc\n";
    os << "\tpop bc\n";
    os << "\tpop bc\n";
    os << "\tpop bc\n";
    break;
  }

  copy(RegisterAllocator::R16_HL, allocation[add]);
}

void AsmPrinter::print_sub(const BinaryOperator *sub) {
  Value *lhs = sub->getOperand(0);
  Value *rhs = sub->getOperand(1);
  int size = load_value(lhs);

  switch (size) {
  case 1:
    load_value(rhs);
    os << "\tsub " << get_register_of(lhs) << ", " << get_register_of(rhs) << "\n";
    break;
  case 2:
    load_value(rhs);
    os << "\tor a\n";
    os << "\tsbc " << get_register_of(lhs) << ", " << get_register_of(rhs) << "\n";
    break;
  case 4:
    os << "\tpush " << get_register_of(lhs).substr(2, 2) << "\n";
    os << "\tpush " << get_register_of(lhs).substr(0, 2) << "\n";
    load_value(rhs);
    os << "\tpush " << get_register_of(rhs).substr(2, 2) << "\n";
    os << "\tpush " << get_register_of(rhs).substr(0, 2) << "\n";
    os << "\tcall __subdi3\n";
    os << "\tpop bc\n";
    os << "\tpop bc\n";
    os << "\tpop bc\n";
    os << "\tpop bc\n";
    break;
  }

  copy(RegisterAllocator::R16_HL, allocation[sub]);
}

void AsmPrinter::print_xor(const BinaryOperator *xor_) {
  Value *lhs = xor_->getOperand(0);
  Value *rhs = xor_->getOperand(1);
  int size = load_value(lhs);
  load_value(rhs);
  
  string lhs_reg = get_register_of(xor_->getOperand(0));
  string rhs_reg = get_register_of(xor_->getOperand(1));
  string target_reg = get_register_of(xor_);

  for (int i = 0; i < size; i++) {
    os << "\tld a, " << lhs_reg[i] << "\n";
    os << "\txor " << rhs_reg[i] << "\n";
    os << "\tld " << target_reg[i] << ", a\n";
  }
}

void AsmPrinter::print_load(const LoadInst *load) {
  load_value(load->getPointerOperand(), allocation[load]);
}

void AsmPrinter::print_store(const StoreInst *store) {
  const Value *value = store->getValueOperand();

  if (auto *constant = dyn_cast<ConstantInt>(value)) {
    os << "\tld " << get_ix(offsets[store->getPointerOperand()]) << ", " << (constant->getSExtValue() & 0xff) << "\n";
    os << "\tld " << get_ix(offsets[store->getPointerOperand()], 1) << ", " << (constant->getSExtValue() >> 8) << "\n";
  } else {
    int offset = offsets[store->getPointerOperand()];
    int size = load_value(value);
    string reg = get_register_of(value);

    for (int i = 0; i < size; i++) {
      os << "\tld " << get_ix(offset, i) << ", " << reg[size - i - 1] << "\n";
    }
  }
}

void AsmPrinter::print_zext(const ZExtInst *zext) {
  string reg = get_register_of(zext->getOperand(0));
  string target_reg = get_register_of(zext);

  copy(allocation[zext->getOperand(0)], allocation[zext]);
  for (int i = reg.length(); i < target_reg.length(); i++) {
    os << "\tld " << target_reg[target_reg.length() - i - 1] << ", 0\n";
  }
}

void AsmPrinter::print_icmp(const ICmpInst *icmp) {
  Value *lhs = icmp->getOperand(0);
  Value *rhs = icmp->getOperand(1);
  int size = load_value(lhs);

  // Set flags
  switch (size) {
  case 1:
    load_value(rhs);
    os << "\tsub " << get_register_of(lhs) << ", " << get_register_of(rhs) << "\n";
    break;
  case 2:
    load_value(rhs);
    os << "\tor a\n";
    os << "\tsbc " << get_register_of(lhs) << ", " << get_register_of(rhs) << "\n";
    break;
  case 4:
    os << "\tpush " << get_register_of(lhs).substr(2, 2) << "\n";
    os << "\tpush " << get_register_of(lhs).substr(0, 2) << "\n";
    load_value(rhs);
    os << "\tpush " << get_register_of(rhs).substr(2, 2) << "\n";
    os << "\tpush " << get_register_of(rhs).substr(0, 2) << "\n";
    os << "\tcall __subdi3\n";
    os << "\tpop bc\n";
    os << "\tpop bc\n";
    os << "\tpop bc\n";
    os << "\tpop bc\n";
    break;
  }

  // Obtain flags by using `jr` for "conditional set"
  os << "\tjr ";
  switch (icmp->getPredicate()) {
  default:
    cerr << "unhandled icmp predicate: " << icmp->getPredicate() << "\n";
    break;
  case ICmpInst::ICMP_EQ:
    os << "nz";
    break;
  case ICmpInst::ICMP_NE:
    os << "\tz";
    break;
  }
  // `jr`(2) + `ld r, n`(2) = 4 bytes
  os << ", . + 4\n";
  os << "\tld " << get_register_of(icmp) << ", 1\n";
}