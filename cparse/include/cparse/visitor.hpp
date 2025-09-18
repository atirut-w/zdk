#pragma once
#include "ast.hpp"
#include <stdexcept>

namespace cparse {

template <typename T> struct Visitor {
  virtual T visit(TranslationUnit &node) {
    visit(*node.function);

    return T();
  }

  virtual T visit(FunctionDefinition &node) {
    visit(*node.body);
    return T();
  }

  virtual T visit(Block &node) {
    for (auto &decl : node.declarations) {
      visit(*decl);
    }
    for (auto &stmt : node.statements) {
      visit(*stmt);
    }
    return T();
  }

  virtual T visit(Declaration &node) { return T(); }

  virtual T visit(Statement &node) {
    if (auto *ret = dynamic_cast<ReturnStatement *>(&node)) {
      return visit(*ret);
    } else if (auto *expr = dynamic_cast<ExpressionStatement *>(&node)) {
      return visit(*expr);
    } else if (auto *ifstmt = dynamic_cast<IfStatement *>(&node)) {
      return visit(*ifstmt);
    } else if (auto *comp = dynamic_cast<CompoundStatement *>(&node)) {
      return visit(*comp);
    } else {
      throw std::runtime_error("BUG: unhandled statement type in visitor");
    }
  }

  virtual T visit(ReturnStatement &node) {
    if (node.expression) {
      visit(*node.expression);
    }
    return T();
  }

  virtual T visit(ExpressionStatement &node) {
    if (node.expression) {
      visit(*node.expression);
    }
    return T();
  }

  virtual T visit(IfStatement &node) {
    if (node.condition) {
      visit(*node.condition);
    }
    if (node.then_branch) {
      visit(*node.then_branch);
    }
    if (node.else_branch) {
      visit(*node.else_branch);
    }
    return T();
  }

  virtual T visit(CompoundStatement &node) {
    if (node.block) {
      visit(*node.block);
    }
    return T();
  }

  virtual T visit(Expression &node) {
    if (auto *const_expr = dynamic_cast<ConstantExpression *>(&node)) {
      return visit(*const_expr);
    } else if (auto *ident_expr = dynamic_cast<IdentifierExpression *>(&node)) {
      return visit(*ident_expr);
    } else if (auto *unary_expr = dynamic_cast<UnaryExpression *>(&node)) {
      return visit(*unary_expr);
    } else if (auto *bin_expr = dynamic_cast<BinaryExpression *>(&node)) {
      return visit(*bin_expr);
    } else if (auto *assign_expr = dynamic_cast<AssignmentExpression *>(&node)) {
      return visit(*assign_expr);
    } else if (auto *cond_expr = dynamic_cast<ConditionalExpression *>(&node)) {
      return visit(*cond_expr);
    } else {
      throw std::runtime_error("BUG: unhandled expression type in visitor");
    }
  }

  virtual T visit(ConstantExpression &node) { return T(); }
  virtual T visit(IdentifierExpression &node) { return T(); }
  
  virtual T visit(UnaryExpression &node) {
    if (node.operand) {
      visit(*node.operand);
    }
    return T();
  }
  
  virtual T visit(BinaryExpression &node) {
    if (node.left) {
      visit(*node.left);
    }
    if (node.right) {
      visit(*node.right);
    }
    return T();
  }

  virtual T visit(AssignmentExpression &node) {
    if (node.left) {
      visit(*node.left);
    }
    if (node.right) {
      visit(*node.right);
    }
    return T();
  }

  virtual T visit(ConditionalExpression &node) {
    if (node.condition) {
      visit(*node.condition);
    }
    if (node.then_expr) {
      visit(*node.then_expr);
    }
    if (node.else_expr) {
      visit(*node.else_expr);
    }
    return T();
  }
};

} // namespace cparse
