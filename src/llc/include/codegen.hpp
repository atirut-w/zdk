#pragma once
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/IR/Instructions.h>
#include <map>
#include <ostream>

class Codegen {
  std::ostream &os;
  llvm::Module *module;

  struct {
    std::map<llvm::Value *, int> locals;
    int stack_size = 0;
  } ctx;

  void load(llvm::Value *value);
  void pregen_function(llvm::Function &func);
  void write_instruction(llvm::Instruction &inst);

  void generate_function(llvm::Function &func);
  void generate_epilogue();
  void generate_store(llvm::StoreInst *store);
  void generate_return(llvm::ReturnInst *ret);

public:
  Codegen(std::ostream &os, llvm::Module *module);

  void generate();
};
