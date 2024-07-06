#pragma once
#include <CBaseVisitor.h>

struct ProgramMeta
{
};

class Analyzer : public CBaseVisitor
{
    ProgramMeta meta;

public:
    virtual std::any visitCompilationUnit(CParser::CompilationUnitContext *ctx) override;
};
