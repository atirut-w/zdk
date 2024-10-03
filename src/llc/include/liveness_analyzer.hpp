#pragma once
#include "ir_visitor.hpp"
#include <llvm/IR/Value.h>
#include <map>
#include <vector>

struct LivenessAnalyzer : public IRVisitor {
  struct Interval {
    int start;
    int end;
    int reg;
  };

private:
  std::map<llvm::Value *, std::vector<Interval>> intervals;
  std::vector<Interval *> active;
};
