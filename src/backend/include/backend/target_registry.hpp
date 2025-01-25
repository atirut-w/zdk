#pragma once
#include "backend/target.hpp"
#include <vector>

class TargetRegistry {
  static std::vector<const Target *> targets;

public:
  static const std::vector<const Target *> &get_targets();

  template <class TargetClass> class Register {
  public:
    Register() {
      static TargetClass target;
      targets.push_back(&target);
    }
  };
};
