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

class Identifier : public Expression {
public:
  std::string name;
};

class Assignment : public Expression {
public:
  std::unique_ptr<Expression> lvalue;
  std::unique_ptr<Expression> rvalue;
};

// Statements

class Statement : public ASTNode {};

class ReturnStatement : public Statement {
public:
  std::unique_ptr<Expression> expression;
};

class ExpressionStatement : public Statement {
public:
  std::unique_ptr<Expression> expression;
};

class IfStatement : public Statement {
public:
  std::unique_ptr<Expression> condition;
  std::unique_ptr<Statement> then_statement;
  std::unique_ptr<Statement> else_statement;
};

class WhileStatement : public Statement {
public:
  std::unique_ptr<Expression> condition;
  std::unique_ptr<Statement> body;
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

class GlobalDeclaration : public ExternalDeclaration {
};

class TranslationUnit : public ASTNode {
public:
  std::vector<std::unique_ptr<ExternalDeclaration>> declarations;
};
