#pragma once
#include "CBaseVisitor.h"
#include "zir/module.hpp"

class IRGen : public CBaseVisitor
{
    ZIR::Module &module;

public:
    IRGen(ZIR::Module &module);
};
