#pragma once
#include <memory>
#include <string>

namespace cparse {

struct ASTNode {
  virtual ~ASTNode() = default;
};

// Expressions

struct Expression : public ASTNode {};

struct ConstantExpression : public Expression {
  int value;
};

struct UnaryExpression : public Expression {
  enum Operator {
    Complement,
    Negate,
  };

  Operator op;
  std::unique_ptr<Expression> operand;
};

struct BinaryExpression : public Expression {
  enum Operator {
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulus,
  };

  Operator op;
  std::unique_ptr<Expression> left;
  std::unique_ptr<Expression> right;
};

// Statements

struct Statement : public ASTNode {};

struct ReturnStatement : public Statement {
  std::unique_ptr<Expression> expression;
};

// Top-level constructs

struct FunctionDefinition : public ASTNode {
  std::string name;
  std::unique_ptr<Statement> body;
};

struct TranslationUnit : public ASTNode {
  std::unique_ptr<FunctionDefinition> function;
};

}
