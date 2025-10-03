#pragma once
#include "cparse/visitor.hpp"
#include <optional>
#include <unordered_map>
#include <vector>

class Analyzer : public cparse::Visitor<void> {
  std::vector<std::string> names;
  int local_start = 0;
  std::string current_function;

  std::optional<std::string> find_symbol(const std::string &name,
                                         bool local = false);

public:
  std::unordered_map<std::string, int> frame_size;

  using cparse::Visitor<void>::visit;

  void visit(cparse::FunctionDefinition &node) override;
  void visit(cparse::Block &node) override;
  void visit(cparse::Declaration &node) override;
  void visit(cparse::IdentifierExpression &node) override;
};
