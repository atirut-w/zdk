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

        output << "\tld hl, " << const_ctx->getText() << "\n";
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return expr_ctx;
}
