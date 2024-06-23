#pragma once
#include <clang-c/Index.h>

/// This class gather the necessary information for codegen such as required stack size, etc.
struct Analyzer
{
    void analyze(CXTranslationUnit tu);

private:
    CXChildVisitResult visit(CXCursor cursor, CXCursor parent, CXClientData client_data);
};
