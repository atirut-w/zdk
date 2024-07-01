#include <analyzer.hpp>
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
    }

    return module_ctx;
}
