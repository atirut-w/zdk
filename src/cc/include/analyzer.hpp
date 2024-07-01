#pragma once
#include <LLVMIRBaseVisitor.h>
#include <string>
#include <map>

struct FunctionInfo
{
    std::string name;
};

struct ModuleInfo
{
    std::string source_file;
    std::map<std::string, FunctionInfo> functions;
};

class Analyzer : public LLVMIRBaseVisitor
{
    ModuleInfo module_ctx;

    virtual std::any visitCompilationUnit(LLVMIRParser::CompilationUnitContext *ctx) override;
    virtual std::any visitFuncDef(LLVMIRParser::FuncDefContext *ctx) override;
};
