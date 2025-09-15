#pragma once
#include <memory>
#include <string>
#include <vector>

namespace cparse {

struct ASTNode {
  virtual ~ASTNode() = default;
};

// Expressions

struct Expression : public ASTNode {};

struct ConstantExpression : public Expression {
  int value;
};

struct IdentifierExpression : public Expression {
  std::string name;
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
    And,
    Or,
    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
  };

  Operator op;
  std::unique_ptr<Expression> left;
  std::unique_ptr<Expression> right;
};

struct AssignmentExpression : public Expression {
  std::unique_ptr<Expression> left;
  std::unique_ptr<Expression> right;
};

struct ConditionalExpression : public Expression {
  std::unique_ptr<Expression> condition;
  std::unique_ptr<Expression> then_expr;
  std::unique_ptr<Expression> else_expr;
};

// Statements

struct Statement : public ASTNode {};

struct ReturnStatement : public Statement {
  std::unique_ptr<Expression> expression;
};

struct ExpressionStatement : public Statement {
  std::unique_ptr<Expression> expression;
};

struct IfStatement : public Statement {
  std::unique_ptr<Expression> condition;
  std::unique_ptr<Statement> then_branch;
  std::unique_ptr<Statement> else_branch;
};

// Top-level constructs

struct Declaration : public ASTNode {
  std::string name;
  std::unique_ptr<Expression> initializer;
};

// Miscellaneous

struct Block : public ASTNode {
  std::vector<std::unique_ptr<Declaration>> declarations;
  std::vector<std::unique_ptr<Statement>> statements;
};

struct CompoundStatement : public Statement {
  std::unique_ptr<Block> block;
};

struct FunctionDefinition : public ASTNode {
  std::string name;
  std::unique_ptr<Block> body;
};

struct TranslationUnit : public ASTNode {
  std::unique_ptr<FunctionDefinition> function;
};

} // namespace cparse
