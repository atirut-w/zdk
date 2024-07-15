#pragma once
#include <CBaseVisitor.h>
#include <map>
#include <string>

struct SymbolMeta
{
    int width = 0;
    bool signedness = true;
};

struct LocalMeta
{
    SymbolMeta symbol;
    int offset;
};

struct FunctionMeta
{
    // Offset into the local frame for local variables
    std::map<std::string, LocalMeta> variables;
    int local_alloc = 0;
};

struct ProgramMeta
{
    std::map<std::string, FunctionMeta> functions;
};

class Analyzer : public CBaseVisitor
{
    ProgramMeta meta;
    FunctionMeta *current_function = nullptr;

public:
    virtual std::any visitCompilationUnit(CParser::CompilationUnitContext *ctx) override;
    virtual std::any visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;
    virtual std::any visitDeclaration(CParser::DeclarationContext *ctx) override;
};
