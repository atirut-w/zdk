#pragma once
#include "backend/target_codegen.hpp"
#include <memory>
#include <string>

class Target {
public:
  std::string name;

  virtual std::unique_ptr<TargetCodegen> create_codegen(std::ostream &os, llvm::Module &module) const = 0;
};
