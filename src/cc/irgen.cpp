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

any IRGen::visitFunction(CParser::FunctionContext *ctx)
{
    string name = ctx->Identifier()->getText();
    
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
        *current_function += Instruction(Instruction::RETURN, operand);
        return operand;
    }
    throw runtime_error("not implemented");
}

any IRGen::visitExpression(CParser::ExpressionContext *ctx)
{
    if (auto *constant = ctx->Constant())
    {
        return Operand(stoi(constant->getText()));
    }
    throw runtime_error("not implemented");
}
