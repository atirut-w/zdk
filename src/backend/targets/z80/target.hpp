#pragma once
#include "backend/target.hpp"

class Z80Target : public Target {
public:
  Z80Target();

  virtual AllocationMap ralloc(const llvm::Function &function) override;
};
