#pragma once
#include "ast.hpp"
#include <map>
#include <ostream>
#include <string>

struct Symbol {
};

struct ExpressionState {
  bool hl_free;
  bool de_free;
};

class Codegen {
  std::ostream &os;
  std::map<std::string, Symbol> symbols;
  int next_label = 0;
  std::vector<ExpressionState> expr_stack;
  ExpressionState expr_state;

  int new_label();
  void add_global(const std::string &name, const Symbol &symbol);

public:
  Codegen(std::ostream &os) : os(os) {}

  void visit(const TranslationUnit &node);
  void visit(const FunctionDefinition &node);
  void visit(const GlobalDeclaration &node);

  void visit(const Statement &node);
  void visit(const ReturnStatement &node);
  void visit(const ExpressionStatement &node);
  void visit(const IfStatement &node);
  void visit(const WhileStatement &node);
  void visit(const ForStatement &node);

  void visit(const Expression &node, bool rhs = false);
  void visit(const IntegerConstant &node, bool rhs = false);
  void visit(const BinaryExpression &node, bool rhs = false);
  void visit(const RelationalExpression &node, bool rhs = false);
  void visit(const IdentifierExpression &node, bool rhs = false);
  void visit(const Assignment &node, bool rhs = false);
};
