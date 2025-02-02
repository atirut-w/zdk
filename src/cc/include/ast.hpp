#pragma once
#include <memory>
#include <string>
#include <vector>

class ASTNode {
public:
  virtual ~ASTNode() = default;
};

// Expressions

class Expression : public ASTNode {};

class BinaryExpression : public Expression {
public:
  enum Operator {
    Add,
    Sub,
    Mul,
    Div,
    Mod,
  };

  std::unique_ptr<Expression> left;
  std::unique_ptr<Expression> right;
  Operator op;
};

class RelationalExpression : public Expression {
public:
  enum Operator {
    Eq,
    Ne,
    Lt,
    Le,
    Gt,
    Ge,
  };

  std::unique_ptr<Expression> left;
  std::unique_ptr<Expression> right;
  Operator op;
};

class IntegerConstant : public Expression {
public:
  int value;
};

// Statements

class Statement : public ASTNode {};

class ReturnStatement : public Statement {
public:
  std::unique_ptr<Expression> expression;
};

// External declarations

class ExternalDeclaration : public ASTNode {
public:
  std::string name;
};

class FunctionDefinition : public ExternalDeclaration {
public:
  std::vector<std::unique_ptr<Statement>> body;
};

class TranslationUnit : public ASTNode {
public:
  std::vector<std::unique_ptr<ExternalDeclaration>> declarations;
};
