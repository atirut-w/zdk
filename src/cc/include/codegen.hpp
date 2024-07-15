#pragma once
#include <CBaseVisitor.h>
#include <analyzer.hpp>
#include <ostream>
#include <variant>

typedef std::variant<char, short, int> ConstantValue;

struct ExpressionCtx
{
    int width;
    bool signedness = true;
    int postfix = 0;
};

class CodeGen : public CBaseVisitor
{
    ProgramMeta &program_meta;
    FunctionMeta *current_function = nullptr;
    std::ostream &output;

    void teardown_frame();

    virtual std::any visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;
    virtual std::any visitJumpStatement(CParser::JumpStatementContext *ctx) override;

    // All of these are for math expressions. Yes, all of them.
    virtual std::any visitPrimaryExpression(CParser::PrimaryExpressionContext *ctx) override; // Your pain starts here
    virtual std::any visitPostfixExpression(CParser::PostfixExpressionContext *ctx) override;
public:
    CodeGen(ProgramMeta &program_meta, std::ostream &output);
};
