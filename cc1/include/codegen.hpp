#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include "ast.hpp"
#include <string>
#include <ostream>

class CodeGenerator : public ASTVisitor {
public:
    virtual ~CodeGenerator() = default;
    virtual void generate(TranslationUnit& tu, std::ostream& out) = 0;
};

#ifdef ENABLE_AMD64_BACKEND
class AMD64CodeGenerator : public CodeGenerator {
public:
    AMD64CodeGenerator();
    void generate(TranslationUnit& tu, std::ostream& out) override;
    
    void visit(IntegerLiteral& node) override;
    void visit(ReturnStatement& node) override;
    void visit(CompoundStatement& node) override;
    void visit(FunctionDeclaration& node) override;
    void visit(TranslationUnit& node) override;
    
private:
    std::ostream* output;
};
#endif

#ifdef ENABLE_Z80_BACKEND
class Z80CodeGenerator : public CodeGenerator {
public:
    Z80CodeGenerator();
    void generate(TranslationUnit& tu, std::ostream& out) override;
    
    void visit(IntegerLiteral& node) override;
    void visit(ReturnStatement& node) override;
    void visit(CompoundStatement& node) override;
    void visit(FunctionDeclaration& node) override;
    void visit(TranslationUnit& node) override;
    
private:
    std::ostream* output;
    int lastValue;
};
#endif

#endif /* CODEGEN_HPP */
