#pragma once
#include "ast.hpp"
#include <any>
#include <ostream>
#include <string>

class Codegen {
  std::ostream &os;
  int used_regs = 0;

  int ralloc();
  int ralloc(int reg);
  void rfree(int reg);

public:
  Codegen(std::ostream &os) : os(os) {}

  void visit(const TranslationUnit &node);
  void visit(const FunctionDefinition &node);

  void visit(const ReturnStatement &node);

  int visit(const Expression &node, int reg = 0);
  int visit(const IntegerConstant &node, int reg = 0);
  int visit(const BinaryExpression &node, int reg = 0);
};
