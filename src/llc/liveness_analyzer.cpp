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

map<Value *, LivenessAnalyzer::IntervalList> LivenessAnalyzer::compute_intervals(Function &func) {
  map<Value *, IntervalList> intervals;
  vector<Instruction *> instructions;

  // Populate the map
  int pos = 0;
  for (auto &block : func) {
    for (auto &inst : block) {
      if (isa<AllocaInst>(inst) || inst.getType()->isVoidTy())
        continue;
      intervals[&inst] = {};

      auto &context = inst.getContext();
      auto *posStr = MDString::get(context, to_string(pos));
      auto *posNode = MDNode::get(context, {posStr});
      inst.setMetadata("pos", posNode);
      instructions.push_back(&inst);
    }
  }

  // Compute the intervals
  map<Value *, Interval> current;
  for (int i = instructions.size() - 1; i >= 0; i--) {
    Instruction *inst = instructions[i];

    for (auto &op : inst->operands()) {
      if (auto *val = dyn_cast<Value>(&op)) {
        if (intervals.find(val) != intervals.end()) {
          if (current.find(val) == current.end()) {
            current[val] = {i, i, -1};
          }
          current[val].start = i;
        }
      }
    }

    if (!inst->getType()->isVoidTy()) {
      if (current.find(inst) != current.end()) {
        current[inst].end = i;
        intervals[inst].push_back(current[inst]);
        current.erase(inst);
      } else {
        intervals[inst].push_back({i, i, -1});
      }
    }
  }

  return intervals;
}
