#pragma once
#include <lexer.hpp>
#include <memory>
#include <vector>

struct ASTNode
{
    std::vector<std::unique_ptr<ASTNode>> children;
    virtual ~ASTNode() = default; // Force polymorphism
};

struct Expression : ASTNode
{
    Token *integer = nullptr;
};

struct Statement : ASTNode
{
    Expression *exp = nullptr;
};

struct FunctionDefinition : ASTNode
{
    Token *identifier = nullptr;
    Statement *body = nullptr;
};

struct Program : ASTNode
{
    FunctionDefinition *main = nullptr;
};

class Parser
{
    std::vector<Token> &tokens;

public:
    Parser(std::vector<Token> &tokens) : tokens(tokens)
    {
    }

    std::unique_ptr<Program> parse();
};
