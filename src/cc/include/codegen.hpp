#pragma once
#include <CBaseVisitor.h>
#include <ostream>

class CodeGen : public CBaseVisitor
{
    std::ostream &os;

    virtual antlrcpp::Any visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;

  public:
    CodeGen(std::ostream &os);
};
