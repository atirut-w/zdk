#include <analyzer.hpp>

using namespace std;
using namespace antlr4;

any Analyzer::visitCompilationUnit(CParser::CompilationUnitContext *ctx)
{
    visitChildren(ctx);
    return meta;
}

any Analyzer::visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx)
{
    string name = ctx->declarator()->directDeclarator()->directDeclarator()->Identifier()->getText();

    if (meta.functions.find(name) == meta.functions.end())
    {
        meta.functions[name] = FunctionMeta();
    }
    current_function = &meta.functions[name];

    return visitChildren(ctx);
}

any Analyzer::visitDeclaration(CParser::DeclarationContext *ctx)
{
    return visitChildren(ctx);
}
