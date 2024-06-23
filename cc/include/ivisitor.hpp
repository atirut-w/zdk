#pragma once
#include <clang-c/Index.h>

struct IVisitor
{
    virtual CXChildVisitResult visit(CXCursor cursor, CXCursor parent) = 0;
    void visit_children(CXCursor cursor);
};
