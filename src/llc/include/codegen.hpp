#pragma once
#include "allocator.hpp"
#include <cstdint>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/IR/Instructions.h>
#include <map>
#include <ostream>

class Codegen {
  std::ostream &os;
  llvm::Module *module;

  struct {
    Allocator allocator;
    std::map<llvm::Value *, int> locals;
    std::map<llvm::Value *, int> loaded;
    int stack_size = 0;
  } ctx;

  void pregen_function(llvm::Function &func);
  void write_instruction(llvm::Instruction &inst);
  void load(llvm::Value *val);
  void vacate(uint8_t regs);
  void assert_regs(uint8_t regs);

  void generate_function(llvm::Function &func);
  void generate_instruction(llvm::Instruction &inst);
  void generate_return(llvm::ReturnInst *ret);
  void generate_epilogue();
  void generate_load(llvm::LoadInst *load);
  void generate_store(llvm::StoreInst *store);

public:
  Codegen(std::ostream &os, llvm::Module *module);

  void generate();
};
