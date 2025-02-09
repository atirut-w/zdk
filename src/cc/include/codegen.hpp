#pragma once
#include "ast.hpp"
#include "symtab.hpp"
#include "type.hpp"
#include <map>
#include <ostream>
#include <string>

class Codegen {
  std::ostream &os;
  Symtab symtab;

  struct {
    int used_regs = 0;
    int label = 0;
  } fctx;

  int ralloc(Type type);
  int ralloc(int regs);
  // int sralloc();
  // int sralloc(int reg);
  bool rused(int regs);
  void rfree(int regs);
  void rcpy(int dst, int src);
  void rsave(int regs = 0);
  void rrestore(int regs = 0);
  int rabi(Type type);

  int new_label();
  // void add_global(const std::string &name, const Symbol &symbol);

public:
  Codegen(std::ostream &os, Symtab symtab) : os(os), symtab(symtab) {}

  void visit(const TranslationUnit &node);
  void visit(const FunctionDefinition &node);
  void visit(const GlobalDeclaration &node);

  void visit(const Statement &node);
  void visit(const ReturnStatement &node);
  void visit(const ExpressionStatement &node);
  void visit(const IfStatement &node);
  void visit(const WhileStatement &node);
  void visit(const ForStatement &node);

  void visit(const Expression &node, int reg);
  void visit(const IntegerConstant &node, int reg);
  void visit(const BinaryExpression &node, int reg);
  void visit(const RelationalExpression &node, int reg);
  void visit(const IdentifierExpression &node, int reg);
  void visit(const Assignment &node, int reg);
  void visit(const CastExpression &node, int reg);
};
