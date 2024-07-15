#include "CParser.h"
#include <analyzer.hpp>

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

    if (meta.functions.find(name) == meta.functions.end())
    {
        meta.functions[name] = FunctionMeta();
    }
    current_function = &meta.functions[name];

    return visitChildren(ctx);
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

    if ((type_spec_ctx->atomicTypeSpecifier() != nullptr) || (type_spec_ctx->structOrUnionSpecifier() != nullptr) || (type_spec_ctx->enumSpecifier() != nullptr) || (type_spec_ctx->typedefName() != nullptr))
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
            if (declarator_ctx->directDeclarator()->Identifier() == nullptr || declarator_ctx->directDeclarator()->DigitSequence() != nullptr)
            {
                throw runtime_error("unsupported declarator");
            }

            current_function->variables[declarator_ctx->directDeclarator()->Identifier()->getText()] = current_function->local_alloc;
            current_function->local_alloc += group_alloc;
        }
    }

    return visitChildren(ctx);
}
