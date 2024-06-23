#include <analyzer.hpp>
#include <iostream>

using namespace std;

CXChildVisitResult Analyzer::visit(CXCursor cursor, CXCursor parent)
{
    return CXChildVisit_Continue; // TODO: Implement this
}
