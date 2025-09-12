#pragma once
#include <ast.hpp>

struct ASTVisitor {
  virtual ~ASTVisitor() = default;

  virtual void visit(TranslationUnit &node);
  virtual void visit(ExternalDeclaration &node);
  virtual void visit(FunctionDeclaration &node);
  virtual void visit(VariableDeclaration &node);
  virtual void visit(Expression &node);
  virtual void visit(Statement &node);
  virtual void visit(BinaryExpression &node);
  virtual void visit(RelationalExpression &node);
  virtual void visit(IntegerConstant &node);
  virtual void visit(IdentifierExpression &node);
  virtual void visit(Assignment &node);
  virtual void visit(FunctionCall &node);
  virtual void visit(ReturnStatement &node);
  virtual void visit(ExpressionStatement &node);
  virtual void visit(IfStatement &node);
  virtual void visit(WhileStatement &node);
  virtual void visit(DoWhileStatement &node);
  virtual void visit(ForStatement &node);
};
