#pragma once
#include <CBaseVisitor.h>
#include <analyzer.hpp>
#include <ostream>

class CodeGen : public CBaseVisitor
{
    ProgramMeta &program_meta;
    std::ostream &output;

public:
    CodeGen(ProgramMeta &program_meta, std::ostream &output);
};
