#pragma once
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <map>
#include <ostream>
#include <string>

class AsmPrinter {
  llvm::Module &module;
  std::ostream &os;

  llvm::Function *current_function = nullptr;
  llvm::BasicBlock *current_block = nullptr;
  std::map<const llvm::Value *, int> offsets;
  std::map<const llvm::BasicBlock *, int> blocknums;
  std::map<const llvm::Value *, int> allocation;

  void generate_prologue();
  std::string get_ix(int base, int offset = 0);
  std::string get_label(const llvm::BasicBlock *block);
  std::string get_register_of(const llvm::Value *value);
  void check_phi(const llvm::BasicBlock *block);
  int load_value(const llvm::Value *value, int reg = 0, bool sign_extend = false);
  void copy(int from, int to);

public:
  AsmPrinter(llvm::Module &module, std::ostream &os) : module(module), os(os) {}

  void print();
  void print_instruction(const llvm::Instruction *instruction);

  void print_return(const llvm::ReturnInst *ret);
  void print_br(const llvm::BranchInst *br);

  void print_add(const llvm::BinaryOperator *add);
  void print_sub(const llvm::BinaryOperator *sub);
  void print_xor(const llvm::BinaryOperator *xor_);

  void print_load(const llvm::LoadInst *load);
  void print_store(const llvm::StoreInst *store);

  void print_zext(const llvm::ZExtInst *zext);

  void print_icmp(const llvm::ICmpInst *icmp);
};
