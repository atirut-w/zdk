#pragma once
#include "liveness_analyzer.hpp"
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <map>
#include <vector>

struct LinearScanAllocator {
  llvm::Module *module;
  std::vector<LivenessAnalyzer::Interval *> intervals;
  std::vector<LivenessAnalyzer::Interval *> active;

  LinearScanAllocator(llvm::Module *module) : module(module) {}

  void allocate(LivenessAnalyzer::IntervalList &intervals);

private:
  int regs = 0;

  void expire(LivenessAnalyzer::Interval *interval);
  void spill(LivenessAnalyzer::Interval *interval);
  int get_usage();

  int allocate_reg(int size);
  void free_reg(int reg);
};
