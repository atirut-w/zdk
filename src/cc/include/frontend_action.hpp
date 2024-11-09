#pragma once
#include "argparse/argparse.hpp"

class FrontendManager;

class FrontendAction {
  int run_command(const std::string &program, const std::vector<std::string> &args);

public:
  virtual ~FrontendAction() = default;
  virtual void register_options(argparse::ArgumentParser &parser) {}
  virtual bool execute(FrontendManager &manager) = 0;
};
