#include <analyzer.hpp>
#include <any>
#include <iostream>

using namespace std;

string unquote(const string &str)
{
    return str.substr(1, str.size() - 2);
}

std::any Analyzer::visitCompilationUnit(LLVMIRParser::CompilationUnitContext *ctx)
{
    for (auto ent : ctx->topLevelEntity())
    {
        if (auto source_fname = ent->sourceFilename())
        {
            module_ctx.source_file = unquote(source_fname->StringLit()->getText());
        }
        else if (auto func_def = ent->funcDef())
        {
            FunctionInfo func_info = any_cast<FunctionInfo>(visitFuncDef(func_def));
            module_ctx.functions[func_info.name] = func_info;
        }
    }

    return module_ctx;
}

std::any Analyzer::visitFuncDef(LLVMIRParser::FuncDefContext *ctx)
{
    FunctionInfo func_info;
    
    return func_info;
}
