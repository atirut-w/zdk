#include "analyzer.hpp"
#include <format>

std::optional<std::string> Analyzer::find_symbol(const std::string &name,
                                                 bool local) {
  for (int i = names.size() - 1; i >= (local ? local_start : 0); --i) {
    if (names[i] == name) {
      return names[i];
    }
  }
  return std::nullopt;
}

void Analyzer::visit(cparse::FunctionDefinition &node) {
  names.push_back(node.name);
  current_function = node.name;
  frame_size[current_function] = 0;
  visit(*node.body);
  names.pop_back();
  current_function.clear();
}

void Analyzer::visit(cparse::Block &node) {
  int prev_local_start = local_start;
  local_start = names.size();

  for (auto &decl : node.declarations) {
    visit(*decl);
  }

  for (auto &stmt : node.statements) {
    visit(*stmt);
  }

  names.resize(local_start);
  local_start = prev_local_start;
}

void Analyzer::visit(cparse::Declaration &node) {
  if (find_symbol(node.name, true)) {
    throw std::runtime_error(
        std::format("Redefinition of symbol '{}'", node.name));
  }
  names.push_back(node.name);
  if (node.initializer) {
    visit(*node.initializer);
  }

  if (!current_function.empty()) {
    frame_size[current_function] += 2;
  }
}

void Analyzer::visit(cparse::IdentifierExpression &node) {
  if (!find_symbol(node.name)) {
    throw std::runtime_error(
        std::format("Use of undeclared symbol '{}'", node.name));
  }
}
