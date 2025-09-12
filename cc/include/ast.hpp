#pragma once
#include <memory>
#include <string>
#include <vector>

struct ASTNode {
  virtual ~ASTNode() = default;
};

// Expressions

struct Expression : public ASTNode {
  bool clobbers_hl = false;
  bool rhs = false;
};

struct BinaryExpression : public Expression {
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

struct RelationalExpression : public Expression {
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

struct IntegerConstant : public Expression {
  int value;
};

struct IdentifierExpression : public Expression {
  std::string name;
};

struct Assignment : public Expression {
  std::unique_ptr<Expression> lvalue;
  std::unique_ptr<Expression> rvalue;
};

struct FunctionCall : public Expression {
  std::string name;
  std::vector<std::unique_ptr<Expression>> arguments;
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
  std::unique_ptr<Statement> then_statement;
  std::unique_ptr<Statement> else_statement;
};

struct WhileStatement : public Statement {
  std::unique_ptr<Expression> condition;
  std::unique_ptr<Statement> body;
};

struct DoWhileStatement : public Statement {
  std::unique_ptr<Statement> body;
  std::unique_ptr<Expression> condition;
};

struct ForStatement : public Statement {
  std::unique_ptr<ASTNode> init;  // Can be Expression or VariableDeclaration
  std::unique_ptr<Expression> condition;
  std::unique_ptr<Expression> update;
  std::unique_ptr<Statement> body;
};


// External declarations

struct ExternalDeclaration : public ASTNode {
  std::string name;
};

struct VariableDeclaration : public ExternalDeclaration {
  std::string type;
  std::unique_ptr<Expression> initializer;
};

struct FunctionDeclaration : public ExternalDeclaration {
  std::string return_type;
  std::vector<std::string> parameters;
  std::vector<std::unique_ptr<ASTNode>> body;  // Can contain both statements and declarations
};

struct TranslationUnit : public ASTNode {
  std::vector<std::unique_ptr<ExternalDeclaration>> declarations;
};
