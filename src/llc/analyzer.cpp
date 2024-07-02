#include <analyzer.hpp>

using namespace std;

any Analyzer::visitCompilationUnit(LLVMIRParser::CompilationUnitContext *ctx)
{
    for (auto ent : ctx->topLevelEntity())
    {
        if (auto func_def = ent->funcDef())
        {
            auto info = any_cast<FunctionInfo>(visitFuncDef(func_def));
            module_info.functions[info.name] = info;
        }
    }

    return module_info;
}

any Analyzer::visitFuncDef(LLVMIRParser::FuncDefContext *ctx)
{
    FunctionInfo info;

    return info;
}
