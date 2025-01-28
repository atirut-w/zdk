#pragma once
#include "backend/target.hpp"
#include <vector>

class TargetRegistry {
  static std::vector<const Target *> targets;

public:
  static const std::vector<const Target *> &get_targets();
  static const Target *get_target(const std::string &name);

  template <class TargetClass> class Register {
  public:
    Register() {
      static TargetClass target;
      targets.push_back(&target);
    }
  };
};
