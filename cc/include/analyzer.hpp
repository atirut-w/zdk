#pragma once
#include <ivisitor.hpp>

/// This class gather the necessary information for codegen such as required stack size, etc.
struct Analyzer : IVisitor
{
    CXChildVisitResult visit(CXCursor cursor, CXCursor parent) override;
};
