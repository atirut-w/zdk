#include "target.hpp"
#include "backend/target_registry.hpp"

TargetRegistry::Register<Z80Target> z80_target;

Z80Target::Z80Target() {
  name = "z80";
}
