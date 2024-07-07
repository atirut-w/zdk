#pragma once
#include <CBaseVisitor.h>
#include <analyzer.hpp>
#include <ostream>
#include <variant>
#include <string>

typedef std::variant<int, float, char> ConstantValue;

struct ExpressionCtx
{
    bool constant; // Used for constant folding
    ConstantValue value;
    std::string symbol;
};

class CodeGen : public CBaseVisitor
{
    ProgramMeta &program_meta;
    std::ostream &output;

public:
    CodeGen(ProgramMeta &program_meta, std::ostream &output);
};
