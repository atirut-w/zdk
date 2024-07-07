#include <cctype>
#include <codegen.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace antlr4;

ConstantValue parse_constant(string text)
{
    // Note: I'm not handling exceptions here, because I trust the ANTLR parser to not let anything weird through.
    if (isdigit(text[0]))
    {
        if (text[0] != '0' || text.size() == 1)
        {
            if (text.find('.') != string::npos)
            {
                throw runtime_error("TODO: figure out floating point math");
                // return stof(text);
            }
            else
            {
                return static_cast<unsigned>(stoi(text));
            }
        }
        else
        {
            char base = tolower(text[1]);
            if (base == 'x' || base == 'X')
            {
                return static_cast<unsigned>(stoi(text, nullptr, 16));
            }
            else if (base == 'b' || base == 'B')
            {
                return static_cast<unsigned>(stoi(text, nullptr, 2));
            }
            else
            {
                return static_cast<unsigned>(stoi(text, nullptr, 8));
            }
        }
    }
    else if (text[0] == '\'')
    {
        // TODO: Unescape characters
        return static_cast<uint8_t>(text[1]);
    }
    else
    {
        throw runtime_error("unhandled constant type");
    }
}

any CodeGen::visitPrimaryExpression(CParser::PrimaryExpressionContext *ctx)
{
    ExpressionCtx expr_ctx;

    if (auto const_ctx = ctx->Constant())
    {
        expr_ctx.value = parse_constant(const_ctx->getText());
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return expr_ctx;
}

any CodeGen::visitPostfixExpression(CParser::PostfixExpressionContext *ctx)
{
    ExpressionCtx expr_ctx;

    if (auto primary_expr_ctx = ctx->primaryExpression())
    {
        expr_ctx = any_cast<ExpressionCtx>(visit(primary_expr_ctx));
    }
    else
    {
        throw runtime_error("struct expression not supported yet");
    }

    return expr_ctx;
}

any CodeGen::visitUnaryExpression(CParser::UnaryExpressionContext *ctx)
{
    ExpressionCtx expr_ctx;

    if (auto postfix_expr_ctx = ctx->postfixExpression())
    {
        expr_ctx = any_cast<ExpressionCtx>(visit(postfix_expr_ctx));
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return expr_ctx;
}

any CodeGen::visitCastExpression(CParser::CastExpressionContext *ctx)
{
    ExpressionCtx expr_ctx;

    if (auto unary_expr_ctx = ctx->unaryExpression())
    {
        expr_ctx = any_cast<ExpressionCtx>(visit(unary_expr_ctx));
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return expr_ctx;
}

any CodeGen::visitMultiplicativeExpression(CParser::MultiplicativeExpressionContext *ctx)
{
    ExpressionCtx expr_ctx;

    if (auto cast_expr_ctx = ctx->castExpression()[0])
    {
        expr_ctx = any_cast<ExpressionCtx>(visit(cast_expr_ctx));
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return expr_ctx;
}

any CodeGen::visitAdditiveExpression(CParser::AdditiveExpressionContext *ctx)
{
    ExpressionCtx expr_ctx;

    if (auto mult_expr_ctx = ctx->multiplicativeExpression()[0])
    {
        expr_ctx = any_cast<ExpressionCtx>(visit(mult_expr_ctx));
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return expr_ctx;
}

any CodeGen::visitShiftExpression(CParser::ShiftExpressionContext *ctx)
{
    ExpressionCtx expr_ctx;

    if (auto add_expr_ctx = ctx->additiveExpression()[0])
    {
        expr_ctx = any_cast<ExpressionCtx>(visit(add_expr_ctx));
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return expr_ctx;
}

any CodeGen::visitRelationalExpression(CParser::RelationalExpressionContext *ctx)
{
    ExpressionCtx expr_ctx;

    if (auto shift_expr_ctx = ctx->shiftExpression()[0])
    {
        expr_ctx = any_cast<ExpressionCtx>(visit(shift_expr_ctx));
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return expr_ctx;
}

any CodeGen::visitEqualityExpression(CParser::EqualityExpressionContext *ctx)
{
    ExpressionCtx expr_ctx;

    if (auto rel_expr_ctx = ctx->relationalExpression()[0])
    {
        expr_ctx = any_cast<ExpressionCtx>(visit(rel_expr_ctx));
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return expr_ctx;
}

any CodeGen::visitAndExpression(CParser::AndExpressionContext *ctx)
{
    ExpressionCtx expr_ctx;

    if (auto eq_expr_ctx = ctx->equalityExpression()[0])
    {
        expr_ctx = any_cast<ExpressionCtx>(visit(eq_expr_ctx));
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return expr_ctx;
}

any CodeGen::visitExclusiveOrExpression(CParser::ExclusiveOrExpressionContext *ctx)
{
    ExpressionCtx expr_ctx;

    if (auto and_expr_ctx = ctx->andExpression()[0])
    {
        expr_ctx = any_cast<ExpressionCtx>(visit(and_expr_ctx));
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return expr_ctx;
}

any CodeGen::visitInclusiveOrExpression(CParser::InclusiveOrExpressionContext *ctx)
{
    ExpressionCtx expr_ctx;

    if (auto excl_or_expr_ctx = ctx->exclusiveOrExpression()[0])
    {
        expr_ctx = any_cast<ExpressionCtx>(visit(excl_or_expr_ctx));
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return expr_ctx;
}

any CodeGen::visitLogicalAndExpression(CParser::LogicalAndExpressionContext *ctx)
{
    ExpressionCtx expr_ctx;

    if (auto incl_or_expr_ctx = ctx->inclusiveOrExpression()[0])
    {
        expr_ctx = any_cast<ExpressionCtx>(visit(incl_or_expr_ctx));
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return expr_ctx;
}

any CodeGen::visitLogicalOrExpression(CParser::LogicalOrExpressionContext *ctx)
{
    ExpressionCtx expr_ctx;

    if (auto log_and_expr_ctx = ctx->logicalAndExpression()[0])
    {
        expr_ctx = any_cast<ExpressionCtx>(visit(log_and_expr_ctx));
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return expr_ctx;
}

any CodeGen::visitConditionalExpression(CParser::ConditionalExpressionContext *ctx)
{
    ExpressionCtx expr_ctx;

    if (auto log_or_expr_ctx = ctx->logicalOrExpression())
    {
        expr_ctx = any_cast<ExpressionCtx>(visit(log_or_expr_ctx));
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return expr_ctx;
}

any CodeGen::visitAssignmentExpression(CParser::AssignmentExpressionContext *ctx)
{
    ExpressionCtx expr_ctx;

    if (auto cond_expr_ctx = ctx->conditionalExpression())
    {
        expr_ctx = any_cast<ExpressionCtx>(visit(cond_expr_ctx));
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return expr_ctx;
}

any CodeGen::visitExpression(CParser::ExpressionContext *ctx)
{
    ExpressionCtx expr_ctx;

    if (auto assign_expr_ctx = ctx->assignmentExpression()[0])
    {
        expr_ctx = any_cast<ExpressionCtx>(visit(assign_expr_ctx));
        if (holds_alternative<string>(expr_ctx.value))
        {
            if (get<string>(expr_ctx.value) == "")
            {
                throw runtime_error("tainted expression not supported yet");
            }
            else
            {
                throw runtime_error("BUG: unresolved reference in expression");
            }
        }
        else
        {
            ConstantValue val = get<ConstantValue>(expr_ctx.value);

            if (holds_alternative<uint8_t>(val))
            {
                output << "\tld a, " << static_cast<unsigned>(get<uint8_t>(val)) << "\n";
            }
            else if (holds_alternative<uint16_t>(val))
            {
                output << "\tld hl, " << static_cast<unsigned>(get<uint16_t>(val)) << "\n";
            }
            else if (holds_alternative<uint32_t>(val))
            {
                uint32_t u32 = get<uint32_t>(val);
                output << "\tld hl, " << (u32 & 0xffff) << "\n";
                output << "\tld de, " << (u32 >> 16) << "\n";
            }
        }
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return expr_ctx;
}
