#pragma once
#include <CBaseVisitor.h>
#include <analyzer.hpp>
#include <ostream>
#include <variant>
#include <string>

typedef std::variant<int, float, char> ConstantValue;

struct ExpressionCtx
{
    bool constant; // Used for constant folding
    ConstantValue value;
    std::string symbol;
};

class CodeGen : public CBaseVisitor
{
    ProgramMeta &program_meta;
    FunctionMeta *current_function = nullptr;
    std::ostream &output;

    virtual std::any visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;

    // All of these are for math expressions. Yes, all of them.
    virtual std::any visitPrimaryExpression(CParser::PrimaryExpressionContext *ctx) override; // Your pain starts here
public:
    CodeGen(ProgramMeta &program_meta, std::ostream &output);
};
