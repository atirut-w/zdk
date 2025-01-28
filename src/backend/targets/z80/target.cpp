#include "target.hpp"
#include "backend/target_registry.hpp"

TargetRegistry::Register<Z80Target> z80_target;

Z80Target::Z80Target() {
  name = "z80";
}

AllocationMap Z80Target::ralloc(const llvm::Function &function) {
  AllocationMap allocation_map;

  // TODO: Implement register allocation for Z80

  return allocation_map;
}
