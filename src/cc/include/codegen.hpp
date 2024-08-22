#pragma once
#include "types.hpp"
#include <CBaseVisitor.h>
#include <analyzer.hpp>
#include <ostream>
#include <variant>
#include <string>
#include <vector>

typedef std::variant<int, float, char> ConstantValue;

struct ExpressionCtx
{
    ParsedType type;
};

struct PrimitiveLayout
{
    // Layout in 8-bit registers
    std::vector<char> registers;
    // Layout in 16-bit register pairs
    std::vector<std::string> pairs;
};

extern const std::vector<PrimitiveLayout> primitive_layouts;

class CodeGen : public CBaseVisitor
{
    std::ostream &output;
    Module &module;

    Function *current_function = nullptr;
    int locals_alloc = 0;
    std::map<std::string, int> local_offsets;

    void teardown_frame();

    virtual std::any visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;
    virtual std::any visitJumpStatement(CParser::JumpStatementContext *ctx) override;

    // All of these are for math expressions. Yes, all of them.
    virtual std::any visitPrimaryExpression(CParser::PrimaryExpressionContext *ctx) override; // Your pain starts here
public:
    CodeGen(Module &module, std::ostream &output);
};
