#pragma once
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
  std::map<const llvm::Value *, int> offsets;

  void generate_prologue();
  std::string get_ix(int base, int offset = 0);
  void load_value(const llvm::Value *value);
  void store_value(const llvm::Value *value);

public:
  AsmPrinter(llvm::Module &module, std::ostream &os) : module(module), os(os) {}

  void print();
  void print_instruction(const llvm::Instruction *instruction);
  void print_load(const llvm::LoadInst *load);
  void print_store(const llvm::StoreInst *store);

  void print_add(const llvm::BinaryOperator *add);
};