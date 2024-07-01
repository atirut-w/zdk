#pragma once
#include <CBaseVisitor.h>
#include <ostream>
#include <string.h>

struct FunctionContext
{
  std::string return_type;
};

class CodeGen : public CBaseVisitor
{
    std::ostream &os;
    FunctionContext current_fn;

    virtual antlrcpp::Any visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) override;
    virtual antlrcpp::Any visitStatement(CParser::StatementContext *ctx) override;

  public:
    CodeGen(std::ostream &os);
};
