#include "CParser.h"
#include "types.hpp"
#include <analyzer.hpp>
#include <any>
#include <memory>
#include <optional>
#include <string>

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

    if (module.functions.find(name) == module.functions.end())
    {
        module.functions[name] = Function();
    }
    current_function = &module.functions[name];
    current_function->return_type = any_cast<ParsedType>(visit(ctx->declarationSpecifiers()));

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
    auto group_type = any_cast<ParsedType>(visit(ctx->declarationSpecifiers()));

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
            local.type = group_type;
            local.offset = current_function->local_alloc;

            current_function->locals[declarator_ctx->directDeclarator()->Identifier()->getText()] = local;
            current_function->local_alloc += group_type->size;
        }

        if (current_function->local_alloc > 0xff)
        {
            throw runtime_error("local variables exceed 8-bit displacement");
        }
    }

    return visitChildren(ctx);
}

any Analyzer::visitDeclarationSpecifiers(CParser::DeclarationSpecifiersContext *ctx)
{
    optional<string> primary_type;
    vector<string> modifiers;

    for (auto *specifier_ctx : ctx->declarationSpecifier())
    {
        if (auto *typespec_ctx = specifier_ctx->typeSpecifier())
        {
            if (typespec_ctx->Long() || typespec_ctx->Float() || typespec_ctx->Double() ||
                typespec_ctx->atomicTypeSpecifier() || typespec_ctx->structOrUnionSpecifier() ||
                typespec_ctx->enumSpecifier() || typespec_ctx->typedefName() || typespec_ctx->getText()[0] == '_')
            {
                throw runtime_error("unsupported type specifier");
            }

            if (typespec_ctx->Void())
            {
                primary_type = "void";
            }
            else if (typespec_ctx->Char())
            {
                primary_type = "char";
            }
            else if (typespec_ctx->Int())
            {
                primary_type = "int";
            }
            else
            {
                // if (find(modifiers.begin(), modifiers.end(), typespec_ctx->getText()) != modifiers.end())
                // {
                //     throw runtime_error("duplicate type specifier");
                // }
                modifiers.push_back(typespec_ctx->getText());
            }
        }
        else
        {
            throw runtime_error("unsupported declaration specifier");
        }
    }

    auto type = make_shared<PrimitiveType>();

    if (primary_type == "void")
    {
        type->size = 0;
    }
    else if (primary_type == "char")
    {
        type->size = 1;
        type->byte_layout = {"a"};
        type->is_signed = find(modifiers.begin(), modifiers.end(), "signed") != modifiers.end();
    }
    else if (primary_type == "int")
    {
        type->size = 2;
        type->byte_layout = {"l", "h"};
        type->word_layout = {"hl"};
        if (find(modifiers.begin(), modifiers.end(), "unsigned") != modifiers.end())
        {
            type->is_signed = false;
        }
    }
    else if (primary_type->empty())
    {
        throw runtime_error("type inference from modifiers not implemented");
    }

    return (ParsedType)type;
}
