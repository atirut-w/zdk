#pragma once
#include <ast_visitor.hpp>

struct Analyzer : public ASTVisitor {
  using ASTVisitor::visit;
  
  void visit(BinaryExpression &node) override;
  void visit(RelationalExpression &node) override;
};
