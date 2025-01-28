#pragma once
#include <llvm/IR/Value.h>
#include <map>

struct RegisterAllocation {
  int reg;
};

typedef std::map<const llvm::Value *, RegisterAllocation> AllocationMap;
