#pragma once
#include <LLVMIRBaseVisitor.h>
#include <string>
#include <vector>

struct ModuleInfo
{
    std::string source_file;
};

class Analyzer : public LLVMIRBaseVisitor
{
    ModuleInfo module_ctx;

    virtual std::any visitCompilationUnit(LLVMIRParser::CompilationUnitContext *ctx) override;
};
