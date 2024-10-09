#include "liveness_analyzer.hpp"
#include <any>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <map>
#include <vector>

using namespace std;
using namespace llvm;

LivenessAnalyzer::IntervalList
LivenessAnalyzer::compute_intervals(Function &func) {
  vector<Instruction *> instructions;

  // Populate the map
  for (auto &block : func) {
    for (auto &inst : block) {
      instructions.push_back(&inst);
    }
  }

  // Compute the intervals
  vector<Interval> intervals;
  map<Value *, Interval> current;
  for (int i = instructions.size() - 1; i >= 0; i--) {
    Instruction *inst = instructions[i];

    for (auto &op : inst->operands()) {
      // Guess what kinda special treatment allocs and voids get
      if (isa<AllocaInst>(op) || op->getType()->isVoidTy()) {
        continue;
      }

      if (auto *val = dyn_cast<Value>(&op)) {
        if (current.find(val) == current.end()) {
          current[val] = {val, -1, i};
        }
      }
    }

    if (!inst->getType()->isVoidTy()) {
      if (current.find(inst) != current.end()) {
        current[inst].start = i;
        intervals.push_back(current[inst]);
        current.erase(inst);
      }
    }
  }

  return intervals;
}
