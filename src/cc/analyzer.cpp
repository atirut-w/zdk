#include "CParser.h"
#include "types.hpp"
#include <analyzer.hpp>
#include <any>
#include <memory>
#include <optional>
#include <string>

using namespace std;
using namespace antlr4;

ParsedType Analyzer::parse_type(CParser::DeclarationSpecifiersContext *ctx, bool no_initlist)
{
    int kind = -1;
    vector<string> seen;
    bool is_signed;

    for (auto *specifier_ctx : ctx->declarationSpecifier())
    {
        if (auto *typespec_ctx = specifier_ctx->typeSpecifier())
        {
            if (specifier_ctx == ctx->declarationSpecifier().back() && typespec_ctx->typedefName() && no_initlist)
            {
                break;
            }
            if (typespec_ctx->Long() || typespec_ctx->Float() || typespec_ctx->Double() ||
                typespec_ctx->atomicTypeSpecifier() || typespec_ctx->structOrUnionSpecifier() ||
                typespec_ctx->enumSpecifier() || typespec_ctx->typedefName() || typespec_ctx->getText()[0] == '_')
            {
                throw runtime_error("unsupported type specifier");
            }

            if (typespec_ctx->Void())
            {
                kind = PrimitiveType::Void;
            }
            else if (typespec_ctx->Char())
            {
                kind = PrimitiveType::Char;
                is_signed = false;
            }
            else if (typespec_ctx->Int())
            {
                kind = PrimitiveType::Int;
                is_signed = true;
            }

            if (find(seen.begin(), seen.end(), typespec_ctx->getText()) != seen.end())
            {
                throw runtime_error("duplicate declaration specifier");
            }
            seen.push_back(typespec_ctx->getText());
        }
        else
        {
            throw runtime_error("unsupported declaration specifier");
        }
    }

    if (kind == -1)
    {
        if (find(seen.begin(), seen.end(), "short") != seen.end())
        {
            kind = PrimitiveType::Short;
            is_signed = true;
        }
        else if (find(seen.begin(), seen.end(), "signed") != seen.end())
        {
            kind = PrimitiveType::Int;
            is_signed = true;
        }
        else if (find(seen.begin(), seen.end(), "unsigned") != seen.end())
        {
            kind = PrimitiveType::Int;
            is_signed = false;
        }
    }
    else
    {
        if (find(seen.begin(), seen.end(), "short") != seen.end())
        {
            if (kind != PrimitiveType::Int)
            {
                throw runtime_error("invalid type specifier");
            }
            kind = PrimitiveType::Short;
        }
        else if (find(seen.begin(), seen.end(), "signed") != seen.end())
        {
            is_signed = true;
        }
        else if (find(seen.begin(), seen.end(), "unsigned") != seen.end())
        {
            is_signed = false;
        }
    }

    auto type = make_shared<PrimitiveType>();
    type->kind = static_cast<PrimitiveType::Kind>(kind);
    type->is_signed = is_signed;

    return type;
}

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
    current_function->return_type = parse_type(ctx->declarationSpecifiers());

    if (auto *itemlist_ctx = ctx->compoundStatement()->blockItemList())
    {
        auto *last_ctx = itemlist_ctx->blockItem().back();
        if (auto *statement_ctx = last_ctx->statement())
        {
            if (auto *jump_ctx = statement_ctx->jumpStatement())
            {
                if (jump_ctx->Return())
                {
                    current_function->has_trailing_return = true;
                }
            }
        }

        for (auto *item_ctx : itemlist_ctx->blockItem())
        {
            visit(item_ctx);
        }
    }

    return any();
}

any Analyzer::visitDeclaration(CParser::DeclarationContext *ctx)
{
    if (auto *init_decl_list_ctx = ctx->initDeclaratorList())
    {
        auto group_type = any_cast<ParsedType>(visit(ctx->declarationSpecifiers()));

        for (auto *init_decl_ctx : init_decl_list_ctx->initDeclarator())
        {
            auto *declarator_ctx = init_decl_ctx->declarator();
            if (declarator_ctx->directDeclarator()->Identifier() == nullptr ||
                declarator_ctx->directDeclarator()->DigitSequence() != nullptr)
            {
                throw runtime_error("unsupported declarator");
            }

            string name = declarator_ctx->directDeclarator()->Identifier()->getText();
            if (current_function->locals.find(name) != current_function->locals.end())
            {
                throw runtime_error("duplicate local declaration");
            }
            current_function->locals[declarator_ctx->directDeclarator()->Identifier()->getText()] = group_type;
        }
    }
    else
    {
        string name = ctx->declarationSpecifiers()->children.back()->getText();
        if (current_function->locals.find(name) != current_function->locals.end())
        {
            throw runtime_error("duplicate local declaration");
        }
        current_function->locals[name] = parse_type(ctx->declarationSpecifiers(), true);
    }

    return any();
}

any Analyzer::visitDeclarationSpecifiers(CParser::DeclarationSpecifiersContext *ctx)
{
    return parse_type(ctx);
}
