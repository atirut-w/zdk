#include <codegen.hpp>

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
                throw runtime_error("floating point arithmetic not supported");
                // return stof(text);
            }
            else
            {
                return stoi(text);
            }
        }
        else
        {
            char base = tolower(text[1]);
            if (base == 'x' || base == 'X')
            {
                return stoi(text, nullptr, 16);
            }
            else if (base == 'b' || base == 'B')
            {
                return stoi(text, nullptr, 2);
            }
            else
            {
                return stoi(text, nullptr, 8);
            }
        }
    }
    else if (text[0] == '\'')
    {
        // TODO: Unescape characters
        return text[1];
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
        ConstantValue value = parse_constant(const_ctx->getText());

        if (holds_alternative<char>(value))
        {
            expr_ctx.width = 1;
            output << "\tld a, " << (int)get<char>(value) << "\n";
        }
        else if (holds_alternative<short>(value))
        {
            expr_ctx.width = 2;
            output << "\tld hl, " << (int)get<short>(value) << "\n";
        }
        else if (holds_alternative<int>(value))
        {
            expr_ctx.width = 4;
            output << "\tld hl, " << (get<int>(value) & 0xffff) << "\n";
            output << "\tld de, " << (get<int>(value) >> 16) << "\n";
        }
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return expr_ctx;
}
