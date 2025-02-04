#pragma once
#include "ast.hpp"
#include <any>
#include <ostream>
#include <string>
#include <vector>

struct Symbol {
  std::string name;
};

class Codegen {
  std::ostream &os;

  std::vector<Symbol> symbols;

  struct {
    int used_regs = 0;
    int label = 0;
  } fctx;

  int ralloc();
  int ralloc(int regs);
  // int sralloc();
  // int sralloc(int reg);
  bool rused(int regs);
  void rfree(int regs);
  void rcpy(int dst, int src);
  void rsave(int regs = 0);
  void rrestore(int regs = 0);

  int new_label();

public:
  Codegen(std::ostream &os) : os(os) {}

  void visit(const TranslationUnit &node);
  void visit(const FunctionDefinition &node);
  void visit(const GlobalDeclaration &node);

  void visit(const ReturnStatement &node);
  void visit(const ExpressionStatement &node);

  void visit(const Expression &node, int reg);
  void visit(const IntegerConstant &node, int reg);
  void visit(const BinaryExpression &node, int reg);
  void visit(const RelationalExpression &node, int reg);
};
