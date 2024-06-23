#include <ivisitor.hpp>

void IVisitor::visit_children(CXCursor cursor)
{
    clang_visitChildren(cursor, [](CXCursor cursor, CXCursor parent, CXClientData client_data) {
        return static_cast<IVisitor *>(client_data)->visit(cursor, parent);
    }, this);
}
