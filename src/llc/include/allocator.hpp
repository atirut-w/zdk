#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>

struct Allocator {
  // Bitfield representing the usage of registers.
  int usage = 0;

  // Allocate a register of specified size.
  int allocate(int size);
  // Allocate a specific register.
  int allocate_register(int mask);
  // Free a register.
  void free(int mask);
  // Find new location for used registers in the given mask.
  // std::vector<std::pair<int, int>> vacate(int mask);
};
