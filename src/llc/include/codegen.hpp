#pragma once
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/IR/Instructions.h>
#include <map>
#include <ostream>

class Codegen {
  std::ostream &os;
  llvm::Module *module;

  struct {
    std::map<llvm::Value *, int> allocs;
    int stack_size = 0;
  } ctx;

  void load_imm(llvm::Value *value);
  void map_allocs(llvm::Function &func);

  void generate_function(llvm::Function &func);
  void generate_store(llvm::StoreInst *store);

public:
  Codegen(std::ostream &os, llvm::Module *module);

  void generate();
};
