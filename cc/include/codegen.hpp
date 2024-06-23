#pragma once
#include <ivisitor.hpp>
#include <analyzer.hpp>
#include <ostream>

class Codegen : public IVisitor
{
private:
    Analyzer &analyzer;
    std::ostream &os;

    CXChildVisitResult visitCompoundStmt(CXCursor cursor, CXCursor parent);

public:
    Codegen(Analyzer &analyzer, std::ostream &os);
    CXChildVisitResult visit(CXCursor cursor, CXCursor parent) override;
};
