#pragma once
#include "CBaseVisitor.h"
#include "zir/module.hpp"

class IRGen : public CBaseVisitor
{
    ZIR::Module &module;
    ZIR::Module::Function *current_function = nullptr;
    struct
    {
        int temp_counter = 0;
    } function_ctx;

    std::string make_temporary();

    virtual std::any visitFunction(CParser::FunctionContext *ctx) override;
    virtual std::any visitStatement(CParser::StatementContext *ctx) override;

    virtual std::any visitExpression(CParser::ExpressionContext *ctx) override;
    virtual std::any visitAdditiveExpression(CParser::AdditiveExpressionContext *ctx) override;
    virtual std::any visitMultiplicativeExpression(CParser::MultiplicativeExpressionContext *ctx) override;
    virtual std::any visitUnaryExpression(CParser::UnaryExpressionContext *ctx) override;
    virtual std::any visitPrimaryExpression(CParser::PrimaryExpressionContext *ctx) override;

public:
    IRGen(ZIR::Module &module);
};
