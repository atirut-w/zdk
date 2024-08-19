#include "CParser.h"
#include "types.hpp"
#include <analyzer.hpp>
#include <any>

using namespace std;
using namespace antlr4;

any Analyzer::visitCompilationUnit(CParser::CompilationUnitContext *ctx)
{
    visitChildren(ctx);
    return module;
}

any Analyzer::visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx)
{
    string name = ctx->declarator()->directDeclarator()->directDeclarator()->Identifier()->getText();
    string return_type = ctx->declarationSpecifiers()->declarationSpecifier()[0]->typeSpecifier()->getText();

    if (module.functions.find(name) == module.functions.end())
    {
        module.functions[name] = Function();
    }
    current_function = &module.functions[name];
    current_function->return_type = &primitives[return_type];

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

    int group_alloc;
    if (type_spec_ctx->Char())
    {
        group_alloc = 1;
    }
    else if (type_spec_ctx->Short())
    {
        group_alloc = 2;
    }
    else if (type_spec_ctx->Int())
    {
        group_alloc = 4;
    }
    else
    {
        throw runtime_error("unsupported type specifier");
    }

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

            Local local;
            local.symbol.width = group_alloc;
            local.offset = current_function->local_alloc;

            current_function->locals[declarator_ctx->directDeclarator()->Identifier()->getText()] = local;
            current_function->local_alloc += group_alloc;
        }

        if (current_function->local_alloc > 0xff)
        {
            throw runtime_error("local variables exceed 8-bit displacement");
        }
    }

    return visitChildren(ctx);
}
