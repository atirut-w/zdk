#pragma once
#include "ir_visitor.hpp"
#include <any>
#include <llvm/IR/Function.h>
#include <llvm/IR/Value.h>
#include <map>
#include <vector>

struct LivenessAnalyzer {
  struct Interval {
    llvm::Value *val;
    int start = -1;
    int end = -1;
    
    int reg = -1;
    bool spilled = false;

    bool operator==(const Interval &other) const {
      return start == other.start && end == other.end && reg == other.reg;
    }
  };

  using IntervalList = std::vector<Interval>;

  IntervalList compute_intervals(llvm::Function &func);

// private:
//   std::map<llvm::Value *, IntervalList> intervals;
};
