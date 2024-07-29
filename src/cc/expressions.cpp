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
                // TODO: Figure out runtime support for floating point arithmetic
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
    current_expression = ExpressionCtx();

    if (auto const_ctx = ctx->Constant())
    {
        ConstantValue value = parse_constant(const_ctx->getText());

        if (holds_alternative<char>(value))
        {
            current_expression->type = &primitives["char"];
            output << "\tld a, " << (int)get<char>(value) << "\n";
        }
        else if (holds_alternative<short>(value))
        {
            current_expression->type = &primitives["short"];
            output << "\tld " << current_expression->type->word_layout[0] << ", " << (int)get<short>(value) << "\n";
        }
        else if (holds_alternative<int>(value))
        {
            current_expression->type = &primitives["int"];
            output << "\tld " << current_expression->type->word_layout[0] << ", " << (get<int>(value) & 0xffff) << "\n";
            output << "\tld " << current_expression->type->word_layout[1] << ", " << (get<int>(value) >> 16) << "\n";
        }
    }
    else if (auto *ident_ctx = ctx->Identifier())
    {
        string name = ident_ctx->getText();
        if (current_function->variables.find(name) == current_function->variables.end())
        {
            throw runtime_error("undeclared variable");
        }

        LocalMeta &local_meta = current_function->variables[name];
        if (auto *primitive = dynamic_cast<PrimitiveType *>(local_meta.type))
        {
            current_expression->type = primitive;
            if (last_local != &local_meta)
            {
                for (int i = 0; i < primitive->size; i++)
                {
                    output << "\tld " << primitive->byte_layout[i] << ", (iy+" << local_meta.offset + i << ")\n";
                }
            }
        }
        else
        {
            throw runtime_error("non-primitive types not supported");
        }
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return any();
}

any CodeGen::visitPostfixExpression(CParser::PostfixExpressionContext *ctx)
{
    // TODO: Test this visitor
    visit(ctx->primaryExpression());

    if (auto primary_expr_ctx = ctx->primaryExpression())
    {
        // TODO: Add complete support for all postfix expressions
        // TODO: Iterate through sequential postfix expressions (e.g. nested structs, etc.)
        if (ctx->expression().size() > 0)
        {
            throw runtime_error("indexing not supported");
        }
        else if (ctx->argumentExpressionList().size() > 0)
        {
            throw runtime_error("I don't even know what this is");
        }
        else if (ctx->Identifier().size() > 0)
        {
            throw runtime_error("struct member access not supported");
        }

        if (ctx->PlusPlus().size() > 0)
        {
            if (ctx->PlusPlus().size() > 1)
            {
                throw runtime_error("multiple increments not supported");
            }
            current_expression->postfix = 1;
        }
        else if (ctx->MinusMinus().size() > 0)
        {
            if (ctx->MinusMinus().size() > 1)
            {
                throw runtime_error("multiple decrements not supported");
            }
            current_expression->postfix = -1;
        }
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return any();
}

any CodeGen::visitUnaryExpression(CParser::UnaryExpressionContext *ctx)
{
    if (auto *postfix_ctx = ctx->postfixExpression())
    {
        visit(postfix_ctx);
    }
    else
    {
        throw runtime_error("unsupported expression type");
    }

    return any();
}
