#pragma once
#include <LLVMIRBaseVisitor.h>

struct ModuleInfo
{
};

class Analyzer : public LLVMIRBaseVisitor
{
    ModuleInfo module_info;

    virtual std::any visitCompilationUnit(LLVMIRParser::CompilationUnitContext *ctx) override;
};
