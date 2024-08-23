#include "types.hpp"
#include <codegen.hpp>

using namespace std;
using namespace antlr4;

any CodeGen::visitPrimaryExpression(CParser::PrimaryExpressionContext *ctx)
{
    ExpressionCtx expr_ctx;

    if (auto const_ctx = ctx->Constant())
    {
        auto type = make_shared<PrimitiveType>();
        type->kind = PrimitiveType::Int;
        expr_ctx.type = type;

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
        throw runtime_error("unsupported expression type");
    }

    return expr_ctx;
}
