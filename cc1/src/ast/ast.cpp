#include "ast.hpp"

void IntegerLiteral::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ReturnStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void CompoundStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void FunctionDeclaration::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void TranslationUnit::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}
