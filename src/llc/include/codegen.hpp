#pragma once
#include <LLVMIRBaseVisitor.h>
#include <analyzer.hpp>
#include <ostream>

class Codegen : public LLVMIRBaseVisitor
{
    ModuleInfo &module_info;
    std::ostream &os;

public:
    Codegen(ModuleInfo &module_info, std::ostream &output);
};
