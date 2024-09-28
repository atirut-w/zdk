#pragma once
#include <llvm/Bitcode/BitcodeReader.h>
#include <map>
#include <ostream>

class Codegen {
  std::ostream &os;
  llvm::Module *module;

  struct {
    std::map<llvm::Value *, int> offsets;
    int stack_size = 0;
  } ctx;

  void load(llvm::Value *value);
  void map_locals(llvm::Function &func);
  void generate_function(llvm::Function &func);

public:
  Codegen(std::ostream &os, llvm::Module *module);

  void generate();
};
