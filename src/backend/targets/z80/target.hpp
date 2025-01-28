#pragma once
#include "backend/target.hpp"

class Z80Target : public Target {
public:
  Z80Target();

  virtual TargetCodegen * create_codegen(std::ostream &os, llvm::Module &module) override;
};
