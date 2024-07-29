#pragma once
#include <CBaseVisitor.h>
#include <analyzer.hpp>
#include <optional>
#include <ostream>
#include <types.hpp>
#include <variant>

typedef std::variant<char, short, int> ConstantValue;

struct ExpressionCtx
{
    PrimitiveType *type;
    bool signedness = true;
    int postfix = 0;
};

class CodeGen : public CBaseVisitor
{
    ProgramMeta &program_meta;
    FunctionMeta *current_function = nullptr;
    std::optional<ExpressionCtx> current_expression;
    LocalMeta *last_local = nullptr;
    std::ostream &output;

    void teardown_frame();
    void primitive_cast(PrimitiveType *to, bool signedness);

    virtual std::any visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;
    virtual std::any visitInitDeclarator(CParser::InitDeclaratorContext *ctx) override;
    virtual std::any visitJumpStatement(CParser::JumpStatementContext *ctx) override;

    // All of these are for math expressions. Yes, all of them.
    virtual std::any visitPrimaryExpression(CParser::PrimaryExpressionContext *ctx) override; // Your pain starts here
    virtual std::any visitPostfixExpression(CParser::PostfixExpressionContext *ctx) override;

public:
    CodeGen(ProgramMeta &program_meta, std::ostream &output);
};
