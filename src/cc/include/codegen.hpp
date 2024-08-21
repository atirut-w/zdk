#pragma once
#include <CBaseVisitor.h>
#include <analyzer.hpp>
#include <ostream>
#include <variant>
#include <string>
#include <vector>

typedef std::variant<int, float, char> ConstantValue;

struct ExpressionCtx
{
    bool constant; // Used for constant folding
    ConstantValue value;
    std::string symbol;
};

struct PrimitiveLayout
{
    // Layout in 8-bit registers
    std::vector<char> registers;
    // Layout in 16-bit register pairs
    std::vector<std::string> pairs;
};

class CodeGen : public CBaseVisitor
{
    Module &program_meta;
    Function *current_function = nullptr;
    std::ostream &output;

    void teardown_frame();

    virtual std::any visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;
    virtual std::any visitJumpStatement(CParser::JumpStatementContext *ctx) override;

    // All of these are for math expressions. Yes, all of them.
    virtual std::any visitPrimaryExpression(CParser::PrimaryExpressionContext *ctx) override; // Your pain starts here
public:
    CodeGen(Module &program_meta, std::ostream &output);
};
