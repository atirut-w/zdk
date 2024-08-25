#include <codegen.hpp>

void Codegen::visit(const ASTNode *node)
{
    if (auto *program = dynamic_cast<const Program *>(node))
    {
        visit(program->function);
    }
    else if (auto *function = dynamic_cast<const FunctionDefinition *>(node))
    {
        os << "\t.global " << function->identifier->text << "\n";
        os << "\t.type " << function->identifier->text << ", @function\n";
        os << function->identifier->text << ":\n";
        visit(function->body);
        os << "\n\n";
    }
    else if (auto *statement = dynamic_cast<const Statement *>(node))
    {
        visit(statement->exp);
        os << "\tret\n";
    }
    else if (auto *expression = dynamic_cast<const Expression *>(node))
    {
        os << "\tld hl, " << expression->integer->text << "\n";
    }
    else
    {
        throw std::runtime_error("Unknown ASTNode type");
    }
}
