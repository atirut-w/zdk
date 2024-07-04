#pragma once
#include <LLVMIRBaseVisitor.h>
#include <map>
#include <string>
#include <vector>

struct FunctionInfo
{
    std::string name;
    std::string type;
    int attribute_group = -1;
    // TODO: Args & locals
};

typedef std::map<std::string, std::string> AttributeGroup;

struct ModuleInfo
{
    std::map<std::string, FunctionInfo> functions;
    std::map<int, AttributeGroup> attribute_groups;
};

class Analyzer : public LLVMIRBaseVisitor
{
    ModuleInfo module_info;

    virtual std::any visitCompilationUnit(LLVMIRParser::CompilationUnitContext *ctx) override;
    virtual std::any visitFuncDef(LLVMIRParser::FuncDefContext *ctx) override;
    virtual std::any visitAttrGroupDef(LLVMIRParser::AttrGroupDefContext *ctx) override;
};
