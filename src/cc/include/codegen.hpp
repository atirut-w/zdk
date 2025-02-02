#pragma once
#include "ast.hpp"
#include <any>
#include <ostream>
#include <string>

class Codegen {
  std::ostream &os;
  int used_regs = 0;
  int label = 0;

  int ralloc();
  int ralloc(int reg);
  // int sralloc();
  // int sralloc(int reg);
  bool rused(int reg);
  void rfree(int reg);
  void rcpy(int dst, int src);

  int new_label();

public:
  Codegen(std::ostream &os) : os(os) {}

  void visit(const TranslationUnit &node);
  void visit(const FunctionDefinition &node);

  void visit(const ReturnStatement &node);
  void visit(const ExpressionStatement &node);

  void visit(const Expression &node, int reg);
  void visit(const IntegerConstant &node, int reg);
  void visit(const BinaryExpression &node, int reg);
  void visit(const RelationalExpression &node, int reg);
};
