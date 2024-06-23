#include <codegen.hpp>
#include <iostream>

using namespace std;

CXChildVisitResult Codegen::visit(CXCursor cursor, CXCursor parent)
{
    CXString name = clang_getCursorSpelling(cursor);
    cout << clang_getCString(name) << endl;
    clang_disposeString(name);

    return CXChildVisit_Recurse;
}
