#include "backend/target_registry.hpp"
#include "targets/z80/target.hpp"
#include <vector>

std::vector<const Target *> TargetRegistry::targets;

const std::vector<const Target *> &TargetRegistry::get_targets() {
  return targets;
}

const Target *TargetRegistry::get_target(const std::string &name) {
  for (const auto &target : targets) {
    if (target->name == name) {
      return target;
    }
  }
  return nullptr;
}
