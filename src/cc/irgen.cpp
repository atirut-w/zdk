#include "irgen.hpp"
#include "zir/instruction.hpp"
#include <memory>
#include <stdexcept>
#include <string>

using namespace std;
using namespace antlr4;
using namespace ZIR;

IRGen::IRGen(Module &module) : module(module)
{
}

string IRGen::make_temporary()
{
    // Must not be a valid C identifier
    string name = to_string(function_ctx.temp_counter++);
    auto symbol = make_unique<Module::Symbol>();
    symbol->name = name;
    current_function->locals.push_back(std::move(symbol));
    return name;
}

any IRGen::visitFunction(CParser::FunctionContext *ctx)
{
    string name = ctx->Identifier()->getText();
    function_ctx = {};
    
    auto func = make_unique<Module::Function>();
    func->name = name;
    current_function = func.get();
    module.symbols.push_back(std::move(func));
    
    return visitChildren(ctx);
}

any IRGen::visitStatement(CParser::StatementContext *ctx)
{
    if (auto *ret = ctx->Return())
    {
        auto operand = any_cast<Operand>(visit(ctx->expression()));
        *current_function += (Instruction(Instruction::RETURN) += operand);
        return operand;
    }
    throw runtime_error("not implemented");
}

any IRGen::visitExpression(CParser::ExpressionContext *ctx)
{
    return visit(ctx->additiveExpression());
}

any IRGen::visitAdditiveExpression(CParser::AdditiveExpressionContext *ctx)
{
    auto lhs = any_cast<Operand>(visit(ctx->multiplicativeExpression(0)));
    if (ctx->multiplicativeExpression().size() == 1)
    {
        return lhs;
    }

    Operand current = lhs;
    for (int i = 1; i < ctx->multiplicativeExpression().size(); i++)
    {
        char op = ctx->children[2 * i - 1]->getText()[0];
        auto rhs = any_cast<Operand>(visit(ctx->multiplicativeExpression(i)));
        Operand tmp = Operand(make_temporary());

        Instruction instruction(Instruction::BINARY, tmp);
        instruction += Operand(op);
        instruction += current;
        instruction += rhs;

        *current_function += instruction;
        current = tmp;
    }

    return current;
}

any IRGen::visitMultiplicativeExpression(CParser::MultiplicativeExpressionContext *ctx)
{
    auto lhs = any_cast<Operand>(visit(ctx->unaryExpression(0)));
    if (ctx->unaryExpression().size() == 1)
    {
        return lhs;
    }

    Operand current = lhs;
    for (int i = 1; i < ctx->unaryExpression().size(); i++)
    {
        char op = ctx->children[2 * i - 1]->getText()[0];
        auto rhs = any_cast<Operand>(visit(ctx->unaryExpression(i)));
        Operand tmp = Operand(make_temporary());

        Instruction instruction(Instruction::BINARY, tmp);
        instruction += Operand(op);
        instruction += current;
        instruction += rhs;

        *current_function += instruction;
        current = tmp;
    }

    return current;
}

any IRGen::visitUnaryExpression(CParser::UnaryExpressionContext *ctx)
{
    auto src = any_cast<Operand>(visit(ctx->primaryExpression()));
    if (ctx->unaryOperator() == nullptr)
    {
        return src;
    }
    Operand dst = Operand(make_temporary());
    Instruction instruction(Instruction::UNARY, dst);
    instruction += Operand(ctx->unaryOperator()->getText()[0]);
    instruction += src;

    *current_function += instruction;
    return dst;
}

any IRGen::visitPrimaryExpression(CParser::PrimaryExpressionContext *ctx)
{
    if (auto *id = ctx->Constant())
    {
        return Operand(stoi(id->getText()));
    }
    else if (auto *subexpr = ctx->expression())
    {
        return visit(subexpr);
    }
    throw runtime_error("not implemented");
}
