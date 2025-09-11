#include "analyzer.hpp"

void Analyzer::visit(BinaryExpression &node) {
  node.clobbers_hl = true;
  node.right->rhs = true;

  visit(*node.left);
  visit(*node.right);
}

void Analyzer::visit(RelationalExpression &node) {
  node.clobbers_hl = true;
  node.right->rhs = true;

  visit(*node.left);
  visit(*node.right);
}
