#include "CLexer.h"
#include "CParser.h"
#include "types.hpp"
#include <codegen.hpp>

using namespace std;
using namespace antlr4;

any CodeGen::visitPostfixExpression(CParser::PostfixExpressionContext *ctx)
{
    ExpressionCtx expr_ctx;
    
    if (auto *primary_expr_ctx = ctx->primaryExpression())
    {
        if (auto *ident_ctx = primary_expr_ctx->Identifier())
        {
            throw runtime_error("symbol lookup not implemented");
        }
        else if (auto *const_ctx = primary_expr_ctx->Constant())
        {
            auto type = make_shared<PrimitiveType>();
            type->kind = PrimitiveType::Int;
            expr_ctx.type = type;
            expr_ctx.loaded = true;

            if (primitive_layouts[type->kind].registers.size() == 1)
            {
                os << "\tld " << primitive_layouts[type->kind].registers[0] << ", " << const_ctx->getText() << "\n";
            }
            else
            {
                for (auto &pair : primitive_layouts[type->kind].pairs)
                {
                    os << "\tld " << pair << ", " << const_ctx->getText() << "\n";
                }
            }
        }
        else
        {
            throw runtime_error("unsupported primary expression type");
        }
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    for (int i = 1; i < ctx->children.size(); i++)
    {
        auto *child = ctx->children[i];

        if (auto *ind_expr_ctx = dynamic_cast<CParser::ExpressionContext *>(child))
        {
            throw runtime_error("indexing not implemented");
        }
        else if (auto *arglist_ctx = dynamic_cast<CParser::ArgumentExpressionListContext *>(child))
        {
            throw runtime_error("function calls not implemented");
        }
        else if (child->getText() == ".")
        {
            throw runtime_error("struct access not implemented");
        }
        else if (child->getText() == "->")
        {
            throw runtime_error("pointer access not implemented");
        }
        else if (child->getText() == "++")
        {
            if (expr_ctx.name.empty())
            {
                throw runtime_error("increment on non-lvalue");
            }
            expr_ctx.postfix = 1;
        }
        else if (child->getText() == "--")
        {
            if (expr_ctx.name.empty())
            {
                throw runtime_error("decrement on non-lvalue");
            }
            expr_ctx.postfix = 2;
        }
        else
        {
            throw runtime_error("how did we get here?");
        }
    }

    return expr_ctx;
}
