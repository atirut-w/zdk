#include "ast_visitor.hpp"

void ASTVisitor::visit(const TranslationUnit &node) {
  for (const auto &decl : node.declarations) {
    visit(*decl);
  }
}

void ASTVisitor::visit(const ExternalDeclaration &node) {
  if (auto *fd = dynamic_cast<const FunctionDefinition *>(&node)) {
    visit(*fd);
  } else if (auto *gd = dynamic_cast<const GlobalDeclaration *>(&node)) {
    visit(*gd);
  }
}

void ASTVisitor::visit(const FunctionDefinition &node) {
  for (const auto &stmt : node.body) {
    if (stmt) {
      visit(*stmt);
    }
  }
}

void ASTVisitor::visit(const Statement &node) {
  if (auto *rs = dynamic_cast<const ReturnStatement *>(&node)) {
    visit(*rs);
  } else if (auto *es = dynamic_cast<const ExpressionStatement *>(&node)) {
    visit(*es);
  } else if (auto *is = dynamic_cast<const IfStatement *>(&node)) {
    visit(*is);
  } else if (auto *ws = dynamic_cast<const WhileStatement *>(&node)) {
    visit(*ws);
  } else if (auto *fs = dynamic_cast<const ForStatement *>(&node)) {
    visit(*fs);
  }
}

void ASTVisitor::visit(const Expression &node) {
  if (auto *ic = dynamic_cast<const IntegerConstant *>(&node)) {
    visit(*ic);
  } else if (auto *cc = dynamic_cast<const CharacterConstant *>(&node)) {
    visit(*cc);
  } else if (auto *be = dynamic_cast<const BinaryExpression *>(&node)) {
    visit(*be);
  } else if (auto *re = dynamic_cast<const RelationalExpression *>(&node)) {
    visit(*re);
  } else if (auto *ie = dynamic_cast<const IdentifierExpression *>(&node)) {
    visit(*ie);
  } else if (auto *as = dynamic_cast<const Assignment *>(&node)) {
    visit(*as);
  } else if (auto *fc = dynamic_cast<const FunctionCall *>(&node)) {
    visit(*fc);
  }
}

void ASTVisitor::visit(const GlobalDeclaration &node) {
  // No traversal needed
}

void ASTVisitor::visit(const BinaryExpression &node) {
  visit(*node.left);
  visit(*node.right);
}

void ASTVisitor::visit(const RelationalExpression &node) {
  visit(*node.left);
  visit(*node.right);
}

void ASTVisitor::visit(const IntegerConstant &node) {
  // Leaf node
}

void ASTVisitor::visit(const CharacterConstant &node) {
  // Leaf node
}

void ASTVisitor::visit(const IdentifierExpression &node) {
  // Leaf node
}

void ASTVisitor::visit(const Assignment &node) {
  visit(*node.lvalue);
  visit(*node.rvalue);
}

void ASTVisitor::visit(const FunctionCall &node) {
  for (const auto &arg : node.arguments) {
    visit(*arg);
  }
}

void ASTVisitor::visit(const ReturnStatement &node) {
  if (node.expression) {
    visit(*node.expression);
  }
}

void ASTVisitor::visit(const ExpressionStatement &node) {
  visit(*node.expression);
}

void ASTVisitor::visit(const IfStatement &node) {
  visit(*node.condition);
  visit(*node.then_statement);
  if (node.else_statement) {
    visit(*node.else_statement);
  }
}

void ASTVisitor::visit(const WhileStatement &node) {
  visit(*node.condition);
  visit(*node.body);
}

void ASTVisitor::visit(const ForStatement &node) {
  if (node.init) {
    visit(*node.init);
  }
  if (node.condition) {
    visit(*node.condition);
  }
  if (node.update) {
    visit(*node.update);
  }
  if (node.body) {
    visit(*node.body);
  }
}
