#pragma once
#include <memory>
#include <string>
#include <vector>

struct ASTNode {
  virtual ~ASTNode() = default;
};

// Expressions

struct Expression : public ASTNode {};

template <typename T> struct PrimaryExpression : public ASTNode {
  T value;
  PrimaryExpression(const T &val) : value(val) {}
};

struct ArrayIndexExpression : public Expression {
  std::unique_ptr<Expression> array;
  std::unique_ptr<Expression> index;
  ArrayIndexExpression(std::unique_ptr<Expression> arr,
                       std::unique_ptr<Expression> idx)
      : array(std::move(arr)), index(std::move(idx)) {}
};

struct FunctionCallExpression : public Expression {
  std::unique_ptr<Expression> function;
  std::vector<std::unique_ptr<Expression>> arguments;
  FunctionCallExpression(std::unique_ptr<Expression> func,
                         std::vector<std::unique_ptr<Expression>> args)
      : function(std::move(func)), arguments(std::move(args)) {}
};

struct MemberAccessExpression : public Expression {
  std::unique_ptr<Expression> object;
  std::string member;
  MemberAccessExpression(std::unique_ptr<Expression> obj,
                         const std::string &mem)
      : object(std::move(obj)), member(mem) {}
};

struct PostfixExpression : public Expression {
  enum class Operator { INCREMENT, DECREMENT };
  std::unique_ptr<Expression> operand;
  Operator op;
  PostfixExpression(std::unique_ptr<Expression> oprnd, Operator oper)
      : operand(std::move(oprnd)), op(oper) {}
};

struct UnaryExpression : public Expression {
  enum class Operator { ADDRESS, DEREFERENCE, PLUS, MINUS, BIT_NOT, LOGIC_NOT };
  std::unique_ptr<Expression> operand;
  Operator op;
  UnaryExpression(std::unique_ptr<Expression> oprnd, Operator oper)
      : operand(std::move(oprnd)), op(oper) {}
};

struct SizeofExpression : public Expression {
  enum class Type { TYPE, EXPRESSION };
  Type type;
  std::unique_ptr<ASTNode> value; // Can be either a type name or an expression
  SizeofExpression(Type t, std::unique_ptr<ASTNode> val)
      : type(t), value(std::move(val)) {}
};

// TODO: CastExpression

struct BinaryExpression : public Expression {
  enum class Operator {
    MULTIPLY,
    DIVIDE,
    MODULO,
    ADD,
    SUBTRACT,
    LEFT_SHIFT,
    RIGHT_SHIFT,
    LESS,
    GREATER,
    LESS_EQUAL,
    GREATER_EQUAL,
    EQUAL,
    NOT_EQUAL,
    BIT_AND,
    BIT_XOR,
    BIT_OR,
    LOGIC_AND,
    LOGIC_OR
  };
  std::unique_ptr<Expression> left;
  std::unique_ptr<Expression> right;
  Operator op;
  BinaryExpression(std::unique_ptr<Expression> lhs,
                   std::unique_ptr<Expression> rhs, Operator oper)
      : left(std::move(lhs)), right(std::move(rhs)), op(oper) {}
};

struct ConditionalExpression : public Expression {
  std::unique_ptr<Expression> condition;
  std::unique_ptr<Expression> true_expr;
  std::unique_ptr<Expression> false_expr;
  ConditionalExpression(std::unique_ptr<Expression> cond,
                        std::unique_ptr<Expression> t_expr,
                        std::unique_ptr<Expression> f_expr)
      : condition(std::move(cond)), true_expr(std::move(t_expr)),
        false_expr(std::move(f_expr)) {}
};

struct AssignmentExpression : public Expression {
  std::unique_ptr<Expression> left;
  std::unique_ptr<Expression> right;
  AssignmentExpression(std::unique_ptr<Expression> lhs,
                       std::unique_ptr<Expression> rhs)
      : left(std::move(lhs)), right(std::move(rhs)) {}
};
