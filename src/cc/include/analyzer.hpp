#pragma once
#include <CBaseVisitor.h>
#include <map>
#include <string>
#include <types.hpp>

struct Symbol
{
    ParsedType type;
};

struct Function : public Symbol
{
    std::map<std::string, ParsedType> locals;
    bool has_trailing_return = false;
};

struct Module
{
    std::map<std::string, Function> functions;
};

class Analyzer : public CBaseVisitor
{
    Module module;
    Function *current_function = nullptr;

    ParsedType parse_type(CParser::DeclarationSpecifiersContext *ctx, bool no_initlist = false);

public:
    virtual std::any visitCompilationUnit(CParser::CompilationUnitContext *ctx) override;
    virtual std::any visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;
    virtual std::any visitDeclaration(CParser::DeclarationContext *ctx) override;
    virtual std::any visitDeclarationSpecifiers(CParser::DeclarationSpecifiersContext *ctx) override;
};
