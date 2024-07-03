#pragma once
#include <LLVMIRBaseVisitor.h>
#include <analyzer.hpp>
#include <ostream>

class Codegen : public LLVMIRBaseVisitor
{
    ModuleInfo &module_info;
    std::ostream &os;
    FunctionInfo *current_function = nullptr;

    virtual std::any visitFuncDef(LLVMIRParser::FuncDefContext *ctx) override;
    virtual std::any visitFuncHeader(LLVMIRParser::FuncHeaderContext *ctx) override;

    virtual std::any visitRetTerm(LLVMIRParser::RetTermContext *ctx) override;

public:
    Codegen(ModuleInfo &module_info, std::ostream &output);
};
