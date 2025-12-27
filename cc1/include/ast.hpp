#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <memory>
#include <vector>

/* Forward declarations */
class ASTVisitor;

/* Base AST node */
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
};

/* Expression nodes */
class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
};

class IntegerLiteral : public Expression {
public:
    int value;
    
    explicit IntegerLiteral(int val) : value(val) {}
    void accept(ASTVisitor& visitor) override;
};

/* Statement nodes */
class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
};

class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> expr;
    
    explicit ReturnStatement(std::unique_ptr<Expression> e) 
        : expr(std::move(e)) {}
    void accept(ASTVisitor& visitor) override;
};

class CompoundStatement : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    
    CompoundStatement() = default;
    void accept(ASTVisitor& visitor) override;
};

/* Declaration nodes */
class FunctionDeclaration : public ASTNode {
public:
    std::string returnType;
    std::string name;
    std::unique_ptr<CompoundStatement> body;
    
    FunctionDeclaration(const std::string& retType, 
                       const std::string& funcName,
                       std::unique_ptr<CompoundStatement> funcBody)
        : returnType(retType), name(funcName), body(std::move(funcBody)) {}
    void accept(ASTVisitor& visitor) override;
};

class TranslationUnit : public ASTNode {
public:
    std::vector<std::unique_ptr<FunctionDeclaration>> functions;
    
    TranslationUnit() = default;
    void accept(ASTVisitor& visitor) override;
};

/* Visitor interface for traversing AST */
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    virtual void visit(IntegerLiteral& node) = 0;
    virtual void visit(ReturnStatement& node) = 0;
    virtual void visit(CompoundStatement& node) = 0;
    virtual void visit(FunctionDeclaration& node) = 0;
    virtual void visit(TranslationUnit& node) = 0;
};

#endif /* AST_HPP */
