#include "linear_scan_allocator.hpp"
#include <algorithm>

using namespace std;
using namespace llvm;

void LinearScanAllocator::allocate(LivenessAnalyzer::IntervalList &intervals) {
  for (auto &interval : intervals) {
    this->intervals.push_back(&interval);
  }

  std::sort(this->intervals.begin(), this->intervals.end(), [](auto *a, auto *b) {
    return a->start < b->start;
  });

  for (auto *interval : this->intervals) {

    expire(interval);
    if (get_usage() >= 7) {
      spill(interval);
    } else {
      Type *type = interval->val->getType();
      uint64_t size = module->getDataLayout().getTypeAllocSize(type);
      int reg = allocator.allocate(size);
      interval->reg = reg;
      
      if (active.empty()) {
        active.push_back(interval);
      } else {
        for (int i = 0; i < active.size(); i++) {
          if (active[i]->end > interval->end) {
            active.insert(active.begin() + i, interval);
            break;
          }
        }
      }
    }
  }
}

void LinearScanAllocator::expire(LivenessAnalyzer::Interval *interval) {
  auto sorted = active;
  std::sort(sorted.begin(), sorted.end(), [](auto *a, auto *b) {
    return a->end < b->end;
  });

  for (auto active_interval : sorted) {
    if (active_interval->end < interval->start) {
      active_interval->spilled = true;
      allocator.free(active_interval->reg);
      active.erase(remove(active.begin(), active.end(), active_interval), active.end());
    }
  }
}

void LinearScanAllocator::spill(LivenessAnalyzer::Interval *interval) {
  if (active.empty()) {
    throw runtime_error("No active intervals to spill");
  }

  // // Get last interval to expire
  // auto spill = active.back();

  // if (spill->end > interval->end) {
  //   interval->reg = spill->reg;
  //   active.erase(remove(active.begin(), active.end(), spill), active.end());
  //   active.push_back(interval);
  // }

  Type *type = interval->val->getType();
  uint64_t required_size = module->getDataLayout().getTypeAllocSize(type);

  while (required_size - get_usage() > 0) {
    LivenessAnalyzer::Interval *spill_interval = active.back();
    allocator.free(spill_interval->reg);
    active.pop_back();
  }

  interval->reg = allocator.allocate(required_size);
}

int LinearScanAllocator::get_usage() {
  int total = 0;
  for (int i = 0; i < 32; i++) {
    if (allocator.usage & (1 << i)) {
      total++;
    }
  }
  return total;
}
