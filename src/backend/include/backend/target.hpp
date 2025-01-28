#pragma once
#include "backend/target_codegen.hpp"
#include <string>

class Target {
public:
  std::string name;

  virtual TargetCodegen *create_codegen(std::ostream &os, llvm::Module &module) = 0;
};
