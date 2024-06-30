#pragma once
#include <CBaseVisitor.h>
#include <ostream>

class CodeGen : public CBaseVisitor
{
    std::ostream &os;

  public:
    CodeGen(std::ostream &os);
};
