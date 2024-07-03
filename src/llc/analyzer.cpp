#include <analyzer.hpp>

using namespace std;

any Analyzer::visitCompilationUnit(LLVMIRParser::CompilationUnitContext *ctx)
{
    module_info = ModuleInfo();
    
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

    auto header = ctx->funcHeader();
    info.type = header->type()->getText();
    info.name = header->GlobalIdent()->getText().substr(1);

    return info;
}
