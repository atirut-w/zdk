#pragma once
#include <CBaseVisitor.h>
#include <map>
#include <string>
#include <types.hpp>

struct Symbol
{
    int width = 0;
    bool signedness = true;
};

struct Local
{
    Symbol symbol;
    int offset;
};

struct Function
{
    Type *return_type;
    // Offset into the local frame for local variables
    std::map<std::string, Local> locals;
    int local_alloc = 0;
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
};
