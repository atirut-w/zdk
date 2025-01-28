#pragma once
#include "backend/target_codegen.hpp"

class Z80Codegen : public TargetCodegen {
protected:
  virtual AllocationMap allocate_registers(llvm::Function &func) override;

public:
  Z80Codegen(std::ostream &os, llvm::Module &module) : TargetCodegen(os, module) {}

  virtual void emit() override;
};
