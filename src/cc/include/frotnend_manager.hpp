#pragma once
#include "argparse/argparse.hpp"
#include "frontend_action.hpp"
#include "frontend_context.hpp"
#include <memory>
#include <vector>

class FrontendManager {
  argparse::ArgumentParser parser;
  std::vector<std::unique_ptr<FrontendAction>> actions;
  FrontendContext context;

public:
  void add_action(std::unique_ptr<FrontendAction> action);
  bool parse_args(int argc, char *argv[]);
  bool run();
  const FrontendContext &get_context() const { return context; }
};
