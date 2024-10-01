#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>

struct RegisterDefinition {
  // The name of the register.
  std::string name;
  // The size of the register, with 1 usually being the smallest size the target
  // can address.
  int size;
  // The unique mask identifying the register and the space it occupies. Must be
  // aligned to its size, and may overlap with other registers similar to how AH
  // and AL forms AX, which then forms the lower half of EAX.
  int mask;
};

class Allocator {
  // Register definitions.
  std::vector<RegisterDefinition> register_defs;

public:
  Allocator(const std::vector<RegisterDefinition> &register_defs);

  // Bitfield representing the usage of registers.
  int usage = 0;

  const std::string &get_register_name(int mask) const;

  // Allocate a register of specified size.
  int allocate(int size);
  // Allocate a specific register.
  int allocate_register(int mask);
  // Free a register.
  void free(int mask);
  // Find new location for used registers in the given mask.
  // std::vector<std::pair<int, int>> vacate(int mask);
};
