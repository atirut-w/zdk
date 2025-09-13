#pragma once
#include <memory>
#include <string>
#include <vector>

struct ASTNode {
  virtual ~ASTNode() = default;
};

// Expressions

struct Expression : public ASTNode {
  bool rhs;
  bool clobbers_hl;
};

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
  bool pointer;
  MemberAccessExpression(std::unique_ptr<Expression> obj,
                         const std::string &mem, bool ptr)
      : object(std::move(obj)), member(mem), pointer(ptr) {}
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

struct SizeofTypeExpression : public Expression {
  std::unique_ptr<TypeName> type_name;
  SizeofTypeExpression(std::unique_ptr<TypeName> type)
      : type_name(std::move(type)) {}
};

struct SizeofExpressionExpression : public Expression {
  std::unique_ptr<Expression> expression;
  SizeofExpressionExpression(std::unique_ptr<Expression> expr)
      : expression(std::move(expr)) {}
};

struct CastExpression : public Expression {
  std::unique_ptr<ASTNode> type_name; // TypeName AST node
  std::unique_ptr<Expression> expression;
  CastExpression(std::unique_ptr<ASTNode> type,
                 std::unique_ptr<Expression> expr)
      : type_name(std::move(type)), expression(std::move(expr)) {}
};

struct PrefixExpression : public Expression {
  enum class Operator { INCREMENT, DECREMENT };
  std::unique_ptr<Expression> operand;
  Operator op;
  PrefixExpression(std::unique_ptr<Expression> oprnd, Operator oper)
      : operand(std::move(oprnd)), op(oper) {}
};

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

struct CommaExpression : public Expression {
  std::vector<std::unique_ptr<Expression>> expressions;
  CommaExpression(std::vector<std::unique_ptr<Expression>> exprs)
      : expressions(std::move(exprs)) {}
};

// Declarations

struct Declaration : public ASTNode {
  std::unique_ptr<DeclarationSpecifiers> declaration_specifiers;
  std::vector<std::unique_ptr<InitDeclarator>> init_declarator_list;
  Declaration(std::unique_ptr<DeclarationSpecifiers> decl_specs,
              std::vector<std::unique_ptr<InitDeclarator>> init_decls)
      : declaration_specifiers(std::move(decl_specs)),
        init_declarator_list(std::move(init_decls)) {}
};

struct DeclarationSpecifiers : public ASTNode {
  std::vector<std::unique_ptr<ASTNode>>
      specifiers; // Mix of storage, type, and qualifiers
  DeclarationSpecifiers(std::vector<std::unique_ptr<ASTNode>> specs)
      : specifiers(std::move(specs)) {}
};

struct InitDeclarator : public ASTNode {
  std::unique_ptr<ASTNode> declarator;
  std::unique_ptr<ASTNode> initializer; // Optional
  InitDeclarator(std::unique_ptr<ASTNode> decl,
                 std::unique_ptr<ASTNode> init = nullptr)
      : declarator(std::move(decl)), initializer(std::move(init)) {}
};

// Storage Class Specifiers
// Keep storage class specifiers as enums - they're simple flags good for
// codegen
struct StorageClassSpecifier : public ASTNode {
  enum class Type { TYPEDEF, EXTERN, STATIC, AUTO, REGISTER };
  Type type;
  StorageClassSpecifier(Type t) : type(t) {}
};

// Type System

// Split type specifiers for better type safety while keeping basic types as
// enum
struct TypeSpecifier : public ASTNode {};

struct BasicTypeSpecifier : public TypeSpecifier {
  enum class Type {
    VOID,
    CHAR,
    SHORT,
    INT,
    LONG,
    FLOAT,
    DOUBLE,
    SIGNED,
    UNSIGNED
  };
  Type type;
  BasicTypeSpecifier(Type t) : type(t) {}
};

struct StructOrUnionTypeSpecifier : public TypeSpecifier {
  std::unique_ptr<StructOrUnionSpecifier> specifier;
  StructOrUnionTypeSpecifier(std::unique_ptr<StructOrUnionSpecifier> spec)
      : specifier(std::move(spec)) {}
};

struct EnumTypeSpecifier : public TypeSpecifier {
  std::unique_ptr<EnumSpecifier> specifier;
  EnumTypeSpecifier(std::unique_ptr<EnumSpecifier> spec)
      : specifier(std::move(spec)) {}
};

struct TypedefNameSpecifier : public TypeSpecifier {
  std::string name;
  TypedefNameSpecifier(const std::string &n) : name(n) {}
};

struct StructOrUnionSpecifier : public ASTNode {
  enum class Type { STRUCT, UNION };
  Type type;
  std::string identifier; // Optional
  std::vector<std::unique_ptr<ASTNode>>
      struct_declarations; // Optional for forward decl
  StructOrUnionSpecifier(Type t, const std::string &id = "",
                         std::vector<std::unique_ptr<ASTNode>> decls = {})
      : type(t), identifier(id), struct_declarations(std::move(decls)) {}
};

struct StructDeclaration : public ASTNode {
  std::unique_ptr<ASTNode> specifier_qualifier_list;
  std::vector<std::unique_ptr<ASTNode>> struct_declarators;
  StructDeclaration(std::unique_ptr<ASTNode> spec_qual,
                    std::vector<std::unique_ptr<ASTNode>> decls)
      : specifier_qualifier_list(std::move(spec_qual)),
        struct_declarators(std::move(decls)) {}
};

struct StructDeclarator : public ASTNode {
  std::unique_ptr<ASTNode> declarator;             // Optional for bit fields
  std::unique_ptr<Expression> constant_expression; // Optional, for bit fields
  StructDeclarator(std::unique_ptr<ASTNode> decl = nullptr,
                   std::unique_ptr<Expression> const_expr = nullptr)
      : declarator(std::move(decl)),
        constant_expression(std::move(const_expr)) {}
};

struct EnumSpecifier : public ASTNode {
  std::string identifier; // Optional
  std::vector<std::unique_ptr<ASTNode>>
      enumerators; // Optional for forward decl
  EnumSpecifier(const std::string &id = "",
                std::vector<std::unique_ptr<ASTNode>> enums = {})
      : identifier(id), enumerators(std::move(enums)) {}
};

struct Enumerator : public ASTNode {
  std::string identifier;
  std::unique_ptr<Expression> constant_expression; // Optional
  Enumerator(const std::string &id,
             std::unique_ptr<Expression> const_expr = nullptr)
      : identifier(id), constant_expression(std::move(const_expr)) {}
};

// Keep type qualifiers as enums - they're simple flags good for codegen
struct TypeQualifier : public ASTNode {
  enum class Type { CONST, VOLATILE };
  Type type;
  TypeQualifier(Type t) : type(t) {}
};

struct Declarator : public ASTNode {
  std::unique_ptr<ASTNode> pointer; // Optional
  std::unique_ptr<ASTNode> direct_declarator;
  Declarator(std::unique_ptr<ASTNode> ptr, std::unique_ptr<ASTNode> direct_decl)
      : pointer(std::move(ptr)), direct_declarator(std::move(direct_decl)) {}
};

struct DirectDeclarator : public ASTNode {
  enum class Type { IDENTIFIER, PAREN, ARRAY, FUNCTION, OLD_FUNCTION };
  Type type;
  std::string identifier;                   // For IDENTIFIER type
  std::unique_ptr<ASTNode> declarator;      // For PAREN type
  std::unique_ptr<ASTNode> base_declarator; // For ARRAY and FUNCTION types
  std::unique_ptr<Expression> array_size;   // For ARRAY type (optional)
  std::unique_ptr<ASTNode> parameter_list;  // For FUNCTION type (optional)
  std::vector<std::string> identifier_list; // For OLD_FUNCTION type (optional)

  DirectDeclarator(Type t, const std::string &id = "")
      : type(t), identifier(id) {}
};

struct Pointer : public ASTNode {
  std::vector<std::unique_ptr<ASTNode>> type_qualifiers;
  std::unique_ptr<ASTNode> pointer; // Optional, for multiple levels
  Pointer(std::vector<std::unique_ptr<ASTNode>> quals = {},
          std::unique_ptr<ASTNode> ptr = nullptr)
      : type_qualifiers(std::move(quals)), pointer(std::move(ptr)) {}
};

struct ParameterTypeList : public ASTNode {
  std::vector<std::unique_ptr<ASTNode>> parameters;
  bool has_ellipsis;
  ParameterTypeList(std::vector<std::unique_ptr<ASTNode>> params,
                    bool ellipsis = false)
      : parameters(std::move(params)), has_ellipsis(ellipsis) {}
};

struct ParameterDeclaration : public ASTNode {
  std::unique_ptr<ASTNode> declaration_specifiers;
  std::unique_ptr<ASTNode> declarator;          // Optional
  std::unique_ptr<ASTNode> abstract_declarator; // Optional
  ParameterDeclaration(std::unique_ptr<ASTNode> decl_specs,
                       std::unique_ptr<ASTNode> decl = nullptr,
                       std::unique_ptr<ASTNode> abs_decl = nullptr)
      : declaration_specifiers(std::move(decl_specs)),
        declarator(std::move(decl)), abstract_declarator(std::move(abs_decl)) {}
};

struct TypeName : public ASTNode {
  std::unique_ptr<ASTNode> specifier_qualifier_list;
  std::unique_ptr<ASTNode> abstract_declarator; // Optional
  TypeName(std::unique_ptr<ASTNode> spec_qual,
           std::unique_ptr<ASTNode> abs_decl = nullptr)
      : specifier_qualifier_list(std::move(spec_qual)),
        abstract_declarator(std::move(abs_decl)) {}
};

struct AbstractDeclarator : public ASTNode {
  std::unique_ptr<ASTNode> pointer;                    // Optional
  std::unique_ptr<ASTNode> direct_abstract_declarator; // Optional
  AbstractDeclarator(std::unique_ptr<ASTNode> ptr = nullptr,
                     std::unique_ptr<ASTNode> direct_abs_decl = nullptr)
      : pointer(std::move(ptr)),
        direct_abstract_declarator(std::move(direct_abs_decl)) {}
};

struct DirectAbstractDeclarator : public ASTNode {
  enum class Type { PAREN, ARRAY, ARRAY_REC, FUNCTION, FUNCTION_REC };
  Type type;
  std::unique_ptr<ASTNode> abstract_declarator; // For PAREN type
  std::unique_ptr<Expression> array_size;       // For ARRAY types (optional)
  std::unique_ptr<ASTNode> base_declarator;     // For recursive types
  std::unique_ptr<ASTNode> parameter_list;      // For FUNCTION types (optional)

  DirectAbstractDeclarator(Type t) : type(t) {}
};

struct Initializer : public ASTNode {};

struct ExpressionInitializer : public Initializer {
  std::unique_ptr<Expression> expression;
  ExpressionInitializer(std::unique_ptr<Expression> expr)
      : expression(std::move(expr)) {}
};

struct ListInitializer : public Initializer {
  std::vector<std::unique_ptr<Initializer>> initializer_list;
  bool trailing_comma;
  ListInitializer(std::vector<std::unique_ptr<Initializer>> init_list,
                  bool comma = false)
      : initializer_list(std::move(init_list)), trailing_comma(comma) {}
};

// Statements

struct Statement : public ASTNode {};

struct LabeledStatement : public Statement {
  std::unique_ptr<Statement> statement;

protected:
  LabeledStatement(std::unique_ptr<Statement> stmt)
      : statement(std::move(stmt)) {}
};

struct IdentifierLabel : public LabeledStatement {
  std::string identifier;
  IdentifierLabel(const std::string &id, std::unique_ptr<Statement> stmt)
      : LabeledStatement(std::move(stmt)), identifier(id) {}
};

struct CaseLabel : public LabeledStatement {
  std::unique_ptr<Expression> constant_expression;
  CaseLabel(std::unique_ptr<Expression> const_expr,
            std::unique_ptr<Statement> stmt)
      : LabeledStatement(std::move(stmt)),
        constant_expression(std::move(const_expr)) {}
};

struct DefaultLabel : public LabeledStatement {
  DefaultLabel(std::unique_ptr<Statement> stmt)
      : LabeledStatement(std::move(stmt)) {}
};

struct CompoundStatement : public Statement {
  std::vector<std::unique_ptr<Declaration>> declarations;
  std::vector<std::unique_ptr<Statement>> statements;
  CompoundStatement(std::vector<std::unique_ptr<Declaration>> decls,
                    std::vector<std::unique_ptr<Statement>> stmts)
      : declarations(std::move(decls)), statements(std::move(stmts)) {}
};

struct ExpressionStatement : public Statement {
  std::unique_ptr<Expression> expression; // Optional
  ExpressionStatement(std::unique_ptr<Expression> expr = nullptr)
      : expression(std::move(expr)) {}
};

struct IfStatement : public Statement {
  std::unique_ptr<Expression> condition;
  std::unique_ptr<Statement> then_statement;
  std::unique_ptr<Statement> else_statement; // Optional

  IfStatement(std::unique_ptr<Expression> cond,
              std::unique_ptr<Statement> then_stmt,
              std::unique_ptr<Statement> else_stmt = nullptr)
      : condition(std::move(cond)), then_statement(std::move(then_stmt)),
        else_statement(std::move(else_stmt)) {}
};

struct SwitchStatement : public Statement {
  std::unique_ptr<Expression> condition;
  std::unique_ptr<Statement> statement;

  SwitchStatement(std::unique_ptr<Expression> cond,
                  std::unique_ptr<Statement> stmt)
      : condition(std::move(cond)), statement(std::move(stmt)) {}
};

struct WhileStatement : public Statement {
  std::unique_ptr<Expression> condition;
  std::unique_ptr<Statement> statement;

  WhileStatement(std::unique_ptr<Expression> cond,
                 std::unique_ptr<Statement> stmt)
      : condition(std::move(cond)), statement(std::move(stmt)) {}
};

struct DoWhileStatement : public Statement {
  std::unique_ptr<Statement> statement;
  std::unique_ptr<Expression> condition;

  DoWhileStatement(std::unique_ptr<Statement> stmt,
                   std::unique_ptr<Expression> cond)
      : statement(std::move(stmt)), condition(std::move(cond)) {}
};

struct ForStatement : public Statement {
  std::unique_ptr<Statement> init;      // ExpressionStatement
  std::unique_ptr<Statement> condition; // ExpressionStatement
  std::unique_ptr<Expression> update;   // Optional
  std::unique_ptr<Statement> statement;

  ForStatement(std::unique_ptr<Statement> init_stmt,
               std::unique_ptr<Statement> cond_stmt,
               std::unique_ptr<Expression> update_expr,
               std::unique_ptr<Statement> stmt)
      : init(std::move(init_stmt)), condition(std::move(cond_stmt)),
        update(std::move(update_expr)), statement(std::move(stmt)) {}
};

struct GotoStatement : public Statement {
  std::string identifier;

  GotoStatement(const std::string &id) : identifier(id) {}
};

struct ContinueStatement : public Statement {};

struct BreakStatement : public Statement {};

struct ReturnStatement : public Statement {
  std::unique_ptr<Expression> expression; // Optional

  ReturnStatement(std::unique_ptr<Expression> expr = nullptr)
      : expression(std::move(expr)) {}
};

// Top-level constructs

struct TranslationUnit : public ASTNode {
  std::vector<std::unique_ptr<ASTNode>> external_declarations;
  TranslationUnit(std::vector<std::unique_ptr<ASTNode>> ext_decls)
      : external_declarations(std::move(ext_decls)) {}
};

// External declarations can be either function definitions or declarations
// We'll use the base ASTNode type for polymorphism in TranslationUnit

struct FunctionDefinition : public ASTNode {
  std::unique_ptr<DeclarationSpecifiers> declaration_specifiers; // Optional
  std::unique_ptr<Declarator> declarator;
  std::vector<std::unique_ptr<Declaration>>
      declarations; // Old-style parameter declarations
  std::unique_ptr<CompoundStatement> compound_statement;

  FunctionDefinition(std::unique_ptr<DeclarationSpecifiers> decl_specs,
                     std::unique_ptr<Declarator> decl,
                     std::vector<std::unique_ptr<Declaration>> old_decls,
                     std::unique_ptr<CompoundStatement> body)
      : declaration_specifiers(std::move(decl_specs)),
        declarator(std::move(decl)), declarations(std::move(old_decls)),
        compound_statement(std::move(body)) {}
};

// Additional helper nodes for better type safety

struct SpecifierQualifierList : public ASTNode {
  std::vector<std::unique_ptr<ASTNode>>
      specifiers; // TypeSpecifiers and TypeQualifiers
  SpecifierQualifierList(std::vector<std::unique_ptr<ASTNode>> specs)
      : specifiers(std::move(specs)) {}
};
