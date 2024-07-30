#include "CParser.h"
#include "types.hpp"
#include <analyzer.hpp>
#include <any>

using namespace std;
using namespace antlr4;

any Analyzer::visitCompilationUnit(CParser::CompilationUnitContext *ctx)
{
    visitChildren(ctx);
    return meta;
}

any Analyzer::visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx)
{
    string name = ctx->declarator()->directDeclarator()->directDeclarator()->Identifier()->getText();
    string return_type = ctx->declarationSpecifiers()->declarationSpecifier()[0]->typeSpecifier()->getText();

    if (meta.functions.find(name) == meta.functions.end())
    {
        meta.functions[name] = FunctionMeta();
    }
    current_function = &meta.functions[name];
    current_function->return_type.type = &primitives[return_type];

    if (auto *itemlist_ctx = ctx->compoundStatement()->blockItemList())
    {
        for (auto *item_ctx : itemlist_ctx->blockItem())
        {
            // We check for return statements here because we only want to check if this function *ends* with a return
            // statement
            if (auto *statement_ctx = item_ctx->statement())
            {
                if (auto *jump_statement_ctx = statement_ctx->jumpStatement())
                {
                    if (jump_statement_ctx->Return())
                    {
                        current_function->has_return = true;
                    }
                }
            }

            visit(item_ctx);
        }
    }

    return any();
}

any Analyzer::visitDeclaration(CParser::DeclarationContext *ctx)
{
    CParser::TypeSpecifierContext *type_spec_ctx;
    for (auto *specifier : ctx->declarationSpecifiers()->declarationSpecifier())
    {
        if ((type_spec_ctx = specifier->typeSpecifier()))
        {
            break;
        }
    }

    if ((type_spec_ctx->atomicTypeSpecifier() != nullptr) || (type_spec_ctx->structOrUnionSpecifier() != nullptr) ||
        (type_spec_ctx->enumSpecifier() != nullptr) || (type_spec_ctx->typedefName() != nullptr))
    {
        throw runtime_error("unsupported type specifier");
    }

    PrimitiveType *group_type;
    if (primitives.find(type_spec_ctx->getText()) == primitives.end())
    {
        throw runtime_error("unsupported type specifier");
    }
    group_type = &(primitives[type_spec_ctx->getText()]);

    // The fact that you can just do `int;` without declaring actual variables just absolutely sends me
    if (auto *init_decl_list_ctx = ctx->initDeclaratorList())
    {
        for (auto *init_decl_ctx : init_decl_list_ctx->initDeclarator())
        {
            auto *declarator_ctx = init_decl_ctx->declarator();
            if (declarator_ctx->directDeclarator()->Identifier() == nullptr ||
                declarator_ctx->directDeclarator()->DigitSequence() != nullptr)
            {
                throw runtime_error("unsupported declarator");
            }

            LocalMeta local;
            local.declaration.type = group_type;
            local.offset = current_function->local_alloc;

            current_function->variables[declarator_ctx->directDeclarator()->Identifier()->getText()] = local;
            current_function->local_alloc += group_type->size;
        }

        if (current_function->local_alloc > 0xff)
        {
            throw runtime_error("local variables exceed 8-bit displacement");
        }
    }

    return visitChildren(ctx);
}
