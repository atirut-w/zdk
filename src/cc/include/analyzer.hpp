#pragma once
#include <LLVMIRBaseVisitor.h>

struct ModuleCtx
{
};

class Analyzer : public LLVMIRBaseVisitor
{
  public:
    ModuleCtx module_ctx;
};
