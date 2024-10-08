#include "linear_scan_allocator.hpp"
#include <algorithm>

using namespace std;
using namespace llvm;

void LinearScanAllocator::allocate(LivenessAnalyzer::IntervalList &intervals) {
  for (auto &interval : intervals) {
    this->intervals.push_back(&interval);
  }

  std::sort(this->intervals.begin(), this->intervals.end(),
            [](auto *a, auto *b) { return a->start < b->start; });

  for (auto *interval : this->intervals) {
    Type *type = interval->val->getType();
    uint64_t size = module->getDataLayout().getTypeAllocSize(type);

    expire(interval);
    if (get_usage() + size > 7) {
      spill(interval);
    } else {
      int reg = allocate_reg(size);
      interval->reg = reg;

      if (active.empty()) {
        active.push_back(interval);
      } else {
        // Sort by increasing end point
        auto it = active.begin();
        while (it != active.end() && (*it)->end < interval->end) {
          it++;
        }
        active.insert(it, interval);
      }
    }
  }
}

void LinearScanAllocator::expire(LivenessAnalyzer::Interval *interval) {
  auto it = active.begin();
  while (it != active.end()) {
    if ((*it)->end < interval->start) {
      free_reg((*it)->reg);
      it = active.erase(it);
    } else {
      ++it;
    }
  }
}

void LinearScanAllocator::spill(LivenessAnalyzer::Interval *interval) {
  if (active.empty()) {
    throw runtime_error("No active intervals to spill");
  }

  auto spill_candidate = active.back();
  if (spill_candidate->end > interval->end) {
    spill_candidate->spilled = true;
    free_reg(spill_candidate->reg);
    active.pop_back();
    interval->reg = spill_candidate->reg;

    auto it = active.begin();
    while (it != active.end() && (*it)->end < interval->end) {
      it++;
    }
    active.insert(it, interval);
  } else {
    interval->spilled = true;
  }
}

int LinearScanAllocator::get_usage() {
  int total = 0;
  for (int i = 0; i < 32; i++) {
    if (regs & (1 << i)) {
      total++;
    }
  }
  return total;
}

int LinearScanAllocator::allocate_reg(int size) {
  switch (size) {
  default:
    throw runtime_error("invalid register allocation size");
  case 1:
    for (int i = 0; i < 7; i++) {
      int mask = 1 << i;
      if (!(regs & mask)) {
        regs |= mask;
        return mask;
      }
    }
    throw runtime_error("out of registers");
  case 2:
    for (int i = 0; i < 3; i++) {
      int mask = 3 << (i * 2);
      if (!(regs & mask)) {
        regs |= mask;
        return mask;
      }
    }
    throw runtime_error("out of registers");
  case 4:
    for (int i = 0; i < 2; i++) {
      int mask = 15 << (i * 2); // It's either BCDE or DEHL
      if (!(regs & mask)) {
        regs |= mask;
        return mask;
      }
    }
    throw runtime_error("out of registers");
  }
}

void LinearScanAllocator::free_reg(int reg) { regs &= ~reg; }
