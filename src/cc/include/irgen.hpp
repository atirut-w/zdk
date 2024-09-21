#pragma once
#include "CBaseVisitor.h"
#include "zir/module.hpp"

class IRGen : public CBaseVisitor
{
    ZIR::Module &module;
    ZIR::Module::Function *current_function = nullptr;

    virtual std::any visitFunction(CParser::FunctionContext *ctx) override;
    virtual std::any visitStatement(CParser::StatementContext *ctx) override;
    virtual std::any visitExpression(CParser::ExpressionContext *ctx) override;

public:
    IRGen(ZIR::Module &module);
};
