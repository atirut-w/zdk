#include <analyzer.hpp>
#include <iostream>

using namespace std;

void Analyzer::analyze(CXTranslationUnit tu)
{
    CXCursor cur = clang_getTranslationUnitCursor(tu);
    clang_visitChildren(cur, [](CXCursor cursor, CXCursor parent, CXClientData client_data) {
        return static_cast<Analyzer *>(client_data)->visit(cursor, parent, client_data);
    }, this);
}

CXChildVisitResult Analyzer::visit(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    CXString name = clang_getCursorSpelling(cursor);
    cout << clang_getCString(name) << endl;
    clang_disposeString(name);

    return CXChildVisit_Recurse;
}
