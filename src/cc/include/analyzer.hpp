#pragma once
#include <CBaseVisitor.h>
#include <map>
#include <string>
#include <types.hpp>

struct Function
{
    ParsedType return_type;
    std::map<std::string, ParsedType> locals;
    bool has_return = false; // TODO: Come up with a better name for this
};

struct Module
{
    std::map<std::string, Function> functions;
};

class Analyzer : public CBaseVisitor
{
    Module module;
    Function *current_function = nullptr;

public:
    virtual std::any visitCompilationUnit(CParser::CompilationUnitContext *ctx) override;
    virtual std::any visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;
    virtual std::any visitDeclaration(CParser::DeclarationContext *ctx) override;
    virtual std::any visitDeclarationSpecifiers(CParser::DeclarationSpecifiersContext *ctx) override;
};
