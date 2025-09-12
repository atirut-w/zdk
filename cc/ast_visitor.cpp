#include "ast_visitor.hpp"

void ASTVisitor::visit(TranslationUnit &node) {
  for (auto &decl : node.declarations) {
    visit(*decl);
  }
}

void ASTVisitor::visit(ExternalDeclaration &node) {
  if (auto *fd = dynamic_cast<FunctionDeclaration *>(&node)) {
    visit(*fd);
  } else if (auto *vd = dynamic_cast<VariableDeclaration *>(&node)) {
    visit(*vd);
  }
}

void ASTVisitor::visit(FunctionDeclaration &node) {
  for (auto &item : node.body) {
    if (auto *stmt = dynamic_cast<Statement *>(item.get())) {
      visit(*stmt);
    } else if (auto *decl = dynamic_cast<ExternalDeclaration *>(item.get())) {
      visit(*decl);
    }
  }
}

void ASTVisitor::visit(Statement &node) {
  if (auto *rs = dynamic_cast<ReturnStatement *>(&node)) {
    visit(*rs);
  } else if (auto *es = dynamic_cast<ExpressionStatement *>(&node)) {
    visit(*es);
  } else if (auto *is = dynamic_cast<IfStatement *>(&node)) {
    visit(*is);
  } else if (auto *ws = dynamic_cast<WhileStatement *>(&node)) {
    visit(*ws);
  } else if (auto *dws = dynamic_cast<DoWhileStatement *>(&node)) {
    visit(*dws);
  } else if (auto *fs = dynamic_cast<ForStatement *>(&node)) {
    visit(*fs);
  }
}

void ASTVisitor::visit(Expression &node) {
  if (auto *ic = dynamic_cast<IntegerConstant *>(&node)) {
    visit(*ic);
  } else if (auto *be = dynamic_cast<BinaryExpression *>(&node)) {
    visit(*be);
  } else if (auto *re = dynamic_cast<RelationalExpression *>(&node)) {
    visit(*re);
  } else if (auto *ie = dynamic_cast<IdentifierExpression *>(&node)) {
    visit(*ie);
  } else if (auto *as = dynamic_cast<Assignment *>(&node)) {
    visit(*as);
  } else if (auto *fc = dynamic_cast<FunctionCall *>(&node)) {
    visit(*fc);
  }
}

void ASTVisitor::visit(VariableDeclaration &node) {
  if (node.initializer) {
    visit(*node.initializer);
  }
}

void ASTVisitor::visit(BinaryExpression &node) {
  visit(*node.left);
  visit(*node.right);
}

void ASTVisitor::visit(RelationalExpression &node) {
  visit(*node.left);
  visit(*node.right);
}

void ASTVisitor::visit(IntegerConstant &node) {
  // Leaf node
}

void ASTVisitor::visit(IdentifierExpression &node) {
  // Leaf node
}

void ASTVisitor::visit(Assignment &node) {
  visit(*node.lvalue);
  visit(*node.rvalue);
}

void ASTVisitor::visit(FunctionCall &node) {
  for (auto &arg : node.arguments) {
    visit(*arg);
  }
}

void ASTVisitor::visit(ReturnStatement &node) {
  if (node.expression) {
    visit(*node.expression);
  }
}

void ASTVisitor::visit(ExpressionStatement &node) {
  visit(*node.expression);
}

void ASTVisitor::visit(IfStatement &node) {
  visit(*node.condition);
  visit(*node.then_statement);
  if (node.else_statement) {
    visit(*node.else_statement);
  }
}

void ASTVisitor::visit(WhileStatement &node) {
  visit(*node.condition);
  visit(*node.body);
}

void ASTVisitor::visit(DoWhileStatement &node) {
  visit(*node.body);
  visit(*node.condition);
}

void ASTVisitor::visit(ForStatement &node) {
  if (node.init) {
    // Handle init as ASTNode - could be Expression or VariableDeclaration
    if (auto *expr = dynamic_cast<Expression *>(node.init.get())) {
      visit(*expr);
    } else if (auto *vd = dynamic_cast<VariableDeclaration *>(node.init.get())) {
      visit(*vd);
    }
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

