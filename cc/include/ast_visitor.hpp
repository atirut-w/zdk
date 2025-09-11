#pragma once
#include <ast.hpp>

struct ASTVisitor {
  virtual ~ASTVisitor() = default;

  virtual void visit(const TranslationUnit &node);
  virtual void visit(const ExternalDeclaration &node);
  virtual void visit(const FunctionDefinition &node);
  virtual void visit(const GlobalDeclaration &node);
  virtual void visit(const Expression &node);
  virtual void visit(const Statement &node);
  virtual void visit(const BinaryExpression &node);
  virtual void visit(const RelationalExpression &node);
  virtual void visit(const IntegerConstant &node);
  virtual void visit(const IdentifierExpression &node);
  virtual void visit(const Assignment &node);
  virtual void visit(const FunctionCall &node);
  virtual void visit(const ReturnStatement &node);
  virtual void visit(const ExpressionStatement &node);
  virtual void visit(const IfStatement &node);
  virtual void visit(const WhileStatement &node);
  virtual void visit(const ForStatement &node);
};
