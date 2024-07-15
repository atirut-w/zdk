#pragma once
#include <CBaseVisitor.h>
#include <map>
#include <string>

struct FunctionMeta
{
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
