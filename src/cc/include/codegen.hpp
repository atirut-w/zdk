#pragma once
#include <CBaseVisitor.h>
#include <analyzer.hpp>
#include <ostream>
#include <optional>
#include <string>
#include <cstdint>

struct ConstantValue
{
    union
    {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
    };

    ConstantValue() : u32(0) {}
    ConstantValue(uint8_t u8) : u8(u8) {}
    ConstantValue(uint16_t u16) : u16(u16) {}
    ConstantValue(uint32_t u32) : u32(u32) {}
};

struct ExpressionCtx
{
    // Empty if no longer applicable for constant folding, i.e. loaded into register(s)
    std::optional<ConstantValue> constant;
    int width = 0;
    int postfix = 0;
};

class CodeGen : public CBaseVisitor
{
    ProgramMeta &program_meta;
    std::ostream &output;

    virtual std::any visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;
    virtual std::any visitJumpStatement(CParser::JumpStatementContext *ctx) override;

    void ensure_expression(ExpressionCtx &ctx);

    // All of these are for math expressions. Yes, all of them.
    virtual std::any visitPrimaryExpression(CParser::PrimaryExpressionContext *ctx) override; // Your pain starts here
    virtual std::any visitPostfixExpression(CParser::PostfixExpressionContext *ctx) override;
    virtual std::any visitUnaryExpression(CParser::UnaryExpressionContext *ctx) override;
    virtual std::any visitCastExpression(CParser::CastExpressionContext *ctx) override;
    virtual std::any visitMultiplicativeExpression(CParser::MultiplicativeExpressionContext *ctx) override;
    virtual std::any visitAdditiveExpression(CParser::AdditiveExpressionContext *ctx) override;
    virtual std::any visitShiftExpression(CParser::ShiftExpressionContext *ctx) override;
    virtual std::any visitRelationalExpression(CParser::RelationalExpressionContext *ctx) override;
    virtual std::any visitEqualityExpression(CParser::EqualityExpressionContext *ctx) override;
    virtual std::any visitAndExpression(CParser::AndExpressionContext *ctx) override;
    virtual std::any visitExclusiveOrExpression(CParser::ExclusiveOrExpressionContext *ctx) override;
    virtual std::any visitInclusiveOrExpression(CParser::InclusiveOrExpressionContext *ctx) override;
    virtual std::any visitLogicalAndExpression(CParser::LogicalAndExpressionContext *ctx) override;
    virtual std::any visitLogicalOrExpression(CParser::LogicalOrExpressionContext *ctx) override;
    virtual std::any visitConditionalExpression(CParser::ConditionalExpressionContext *ctx) override;
    virtual std::any visitAssignmentExpression(CParser::AssignmentExpressionContext *ctx) override;
    virtual std::any visitExpression(CParser::ExpressionContext *ctx) override;
public:
    CodeGen(ProgramMeta &program_meta, std::ostream &output);
};
