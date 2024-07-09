#include <cctype>
#include <codegen.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace antlr4;

ExpressionCtx parse_constant(string text)
{
    ExpressionCtx expr_ctx;

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
                expr_ctx.width = 4;
                expr_ctx.constant = static_cast<unsigned>(stoi(text));
            }
        }
        else
        {
            char base = tolower(text[1]);
            if (base == 'x' || base == 'X')
            {
                expr_ctx.width = 4;
                expr_ctx.constant = static_cast<unsigned>(stoi(text, nullptr, 16));
            }
            else if (base == 'b' || base == 'B')
            {
                expr_ctx.width = 4;
                expr_ctx.constant = static_cast<unsigned>(stoi(text, nullptr, 2));
            }
            else
            {
                expr_ctx.width = 4;
                expr_ctx.constant = static_cast<unsigned>(stoi(text, nullptr, 8));
            }
        }
    }
    else if (text[0] == '\'')
    {
        // TODO: Unescape characters
        expr_ctx.width = 1;
        expr_ctx.constant = static_cast<uint8_t>(text[1]);
    }
    else
    {
        throw runtime_error("unhandled constant type");
    }

    return expr_ctx;
}

void CodeGen::ensure_expression(ExpressionCtx &ctx)
{
    if (ctx.constant)
    {
        if (ctx.width == 1)
        {
            output << "\tld a, " << static_cast<unsigned>(ctx.constant->u8) << "\n";
        }
        else if (ctx.width == 2)
        {
            output << "\tld hl, " << static_cast<unsigned>(ctx.constant->u16) << "\n";
        }
        else if (ctx.width == 4)
        {
            output << "\tld hl, " << (ctx.constant->u32 & 0xffff) << "\n";
            output << "\tld de, " << (ctx.constant->u32 >> 16) << "\n";
        }

        ctx.constant = nullopt;
    }
}

void CodeGen::promote_expression(ExpressionCtx &ctx)
{
    if (!ctx.constant)
    {
        if (!ctx.signedness)
        {
            if (ctx.width == 1)
            {
                output << "\tld h, 0\n";
                output << "\tld l, a\n";
            }
            else if (ctx.width == 2)
            {
                output << "\tld de, 0\n";
            }
        }
        else
        {
            if (ctx.width == 1)
            {
                output << "\tcall _crt_sext_8\n";
            }
            else if (ctx.width == 2)
            {
                output << "\tcall _crt_sext_16\n";
            }
        }
    }
    else
    {
        if (ctx.width == 1)
        {
            if (!ctx.signedness)
                ctx.constant->u32 = ctx.constant->u8;
            else
                ctx.constant->i32 = static_cast<int8_t>(ctx.constant->u8);
        }
        else if (ctx.width == 2)
        {
            if (!ctx.signedness)
                ctx.constant->u32 = ctx.constant->u16;
            else
                ctx.constant->i32 = static_cast<int16_t>(ctx.constant->u16);
        }
    }

    ctx.width = 4;
}

any CodeGen::visitPrimaryExpression(CParser::PrimaryExpressionContext *ctx)
{
    ExpressionCtx expr_ctx;

    if (auto const_ctx = ctx->Constant())
    {
        expr_ctx = parse_constant(const_ctx->getText());
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
    ExpressionCtx expr_ctx = any_cast<ExpressionCtx>(visit(ctx->multiplicativeExpression()[0]));
    promote_expression(expr_ctx);

    if (!expr_ctx.constant)
    {
        throw runtime_error("tainted expression not supported yet");
    }
    else
    {
        ConstantValue val = *expr_ctx.constant;

        for (int i = 1; i < ctx->multiplicativeExpression().size(); i++)
        {
            ExpressionCtx add_expr_ctx = any_cast<ExpressionCtx>(visit(ctx->multiplicativeExpression()[i]));
            promote_expression(add_expr_ctx);

            if (!add_expr_ctx.signedness)
            {
                expr_ctx.signedness = false;
            }

            if (!add_expr_ctx.constant)
            {
                throw runtime_error("tainted expression not supported yet");
            }
            else
            {
                ConstantValue add_val = *add_expr_ctx.constant;
                char op = ctx->children[2 * i - 1]->getText()[0];

                if (expr_ctx.signedness)
                {
                    if (op == '+')
                    {
                        val.i32 += add_val.i32;
                    }
                    else if (op == '-')
                    {
                        val.i32 -= add_val.i32;
                    }
                }
                else
                {
                    if (op == '+')
                    {
                        val.u32 += add_val.u32;
                    }
                    else if (op == '-')
                    {
                        val.u32 -= add_val.u32;
                    }
                }
            }
        }

        expr_ctx.constant = val;
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
        ensure_expression(expr_ctx);
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return expr_ctx;
}
