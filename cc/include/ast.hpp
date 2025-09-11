#pragma once
#include <memory>
#include <string>

struct ASTNode {
  virtual ~ASTNode() = default;
};

// Expressions

struct Expression : public ASTNode {};

struct Integer : public Expression {
  int value;
};

// Statements

struct Statement : public ASTNode {};

struct ReturnStatement : public Statement {
  std::unique_ptr<Expression> expr;
};

// Top-level constructs

struct Function : public ASTNode {
  std::string name;
  std::unique_ptr<Statement> body;
};

struct Program : public ASTNode {
  std::unique_ptr<Function> function;
};
