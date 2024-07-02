#include <analyzer.hpp>

using namespace std;

any Analyzer::visitCompilationUnit(LLVMIRParser::CompilationUnitContext *ctx)
{
    return module_info;
}
