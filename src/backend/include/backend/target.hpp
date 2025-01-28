#pragma once
#include "backend/ralloc.hpp"
#include <string>

class Target {
public:
  std::string name;

  virtual AllocationMap ralloc(const llvm::Function &function) = 0;
};
