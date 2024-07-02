#pragma once
#include <LLVMIRBaseVisitor.h>
#include <analyzer.hpp>
#include <ostream>

class Compiler : public LLVMIRBaseVisitor
{
    ModuleInfo &module_info;
    std::ostream &os;

  public:
    Compiler(ModuleInfo &module_info, std::ostream &output);
};
