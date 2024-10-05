#pragma once
#include "allocator.hpp"
#include "liveness_analyzer.hpp"
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <map>
#include <vector>

struct LinearScanAllocator {
  llvm::Module *module;
  std::vector<LivenessAnalyzer::Interval *> intervals;
  std::vector<LivenessAnalyzer::Interval *> active;
  Allocator allocator;

  LinearScanAllocator(llvm::Module *module) : module(module) {}

  void allocate(LivenessAnalyzer::IntervalList &intervals);

private:
  void expire(LivenessAnalyzer::Interval *interval);
  void spill(LivenessAnalyzer::Interval *interval);
  int get_usage();
};
