#pragma once
#include "ast.hpp"
#include "ast_visitor.hpp"
#include <map>
#include <ostream>
#include <string>

struct Symbol {
};

class Codegen : public ASTVisitor {
  std::ostream &os;
  std::map<std::string, Symbol> symbols;
  int next_label = 0;

  int new_label();
  void add_global(const std::string &name, const Symbol &symbol);

public:
  Codegen(std::ostream &os) : os(os) {}

  using ASTVisitor::visit;

  void visit(FunctionDefinition &node) override;
  void visit(GlobalDeclaration &node) override;

  void visit(ReturnStatement &node) override;
  void visit(ExpressionStatement &node) override;
  void visit(IfStatement &node) override;
  void visit(WhileStatement &node) override;
  void visit(ForStatement &node) override;

  void visit(IntegerConstant &node) override;
  void visit(BinaryExpression &node) override;
  void visit(RelationalExpression &node) override;
  void visit(IdentifierExpression &node) override;
  void visit(Assignment &node) override;
};
