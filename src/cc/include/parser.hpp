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
    FunctionDefinition *function = nullptr;
};

struct ParserError : std::runtime_error, Positional
{
    ParserError(Positional &ctx, const std::string &message) : std::runtime_error(message), Positional(ctx)
    {
    }
};

class Parser : Positional
{
    std::vector<Token> &tokens;
    int index = 0;

public:
    Parser(std::vector<Token> &tokens) : tokens(tokens)
    {
    }

    Token *expect(Token::Type type);
    Token *expect(const std::string &text);

    std::unique_ptr<Program> parse();
    FunctionDefinition *parse_function();
    Statement *parse_statement();
    Expression *parse_expression();
};
