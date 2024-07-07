#pragma once
#include <CBaseVisitor.h>
#include <analyzer.hpp>
#include <ostream>
#include <variant>
#include <string>
#include <cstdint>

struct ConstantValue
{
    int width;
    union
    {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
    };

    ConstantValue() : width(0) {}
    ConstantValue(uint8_t value) : width(1), u8(value) {}
    ConstantValue(uint16_t value) : width(2), u16(value) {}
    ConstantValue(uint32_t value) : width(4), u32(value) {}
};

// A value representing an expression's result. Contains an empty string when already loaded, and a symbol name when not a constant.
typedef std::variant<ConstantValue, std::string> ExpressionValue;

struct ExpressionCtx
{
    ExpressionValue value;
    int postfix = 0;
};

class CodeGen : public CBaseVisitor
{
    ProgramMeta &program_meta;
    std::ostream &output;

    virtual std::any visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;
    virtual std::any visitJumpStatement(CParser::JumpStatementContext *ctx) override;

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
