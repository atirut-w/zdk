#pragma once
#include "backend/target.hpp"

class Z80Target : public Target {
public:
  Z80Target();

  virtual std::unique_ptr<TargetCodegen> create_codegen(std::ostream &os, llvm::Module &module) const override;
};
