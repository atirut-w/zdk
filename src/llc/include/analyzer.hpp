#pragma once
#include <LLVMIRBaseVisitor.h>
#include <map>
#include <string>

struct FunctionInfo
{
    std::string name;
    std::string type;
    // TODO: Args & locals
};

struct ModuleInfo
{
    std::map<std::string, FunctionInfo> functions;
};

class Analyzer : public LLVMIRBaseVisitor
{
    ModuleInfo module_info;

    virtual std::any visitCompilationUnit(LLVMIRParser::CompilationUnitContext *ctx) override;
    virtual std::any visitFuncDef(LLVMIRParser::FuncDefContext *ctx) override;
};
