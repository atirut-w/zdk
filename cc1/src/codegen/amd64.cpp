#include "codegen.hpp"

#ifdef ENABLE_AMD64_BACKEND

AMD64CodeGenerator::AMD64CodeGenerator() : output(nullptr) {}

void AMD64CodeGenerator::generate(TranslationUnit& tu, std::ostream& out) {
    output = &out;
    tu.accept(*this);
}

void AMD64CodeGenerator::visit(IntegerLiteral& node) {
    /* Move immediate value to rax */
    *output << "    movl $" << node.value << ", %eax\n";
}

void AMD64CodeGenerator::visit(ReturnStatement& node) {
    /* Evaluate the return expression */
    if (node.expr) {
        node.expr->accept(*this);
    }
    
    /* Function epilogue */
    *output << "    popq %rbp\n";
    *output << "    ret\n";
}

void AMD64CodeGenerator::visit(CompoundStatement& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
}

void AMD64CodeGenerator::visit(FunctionDeclaration& node) {
    /* Emit function label */
    *output << "    .globl " << node.name << "\n";
    *output << "    .type " << node.name << ", @function\n";
    *output << node.name << ":\n";
    
    /* Function prologue (SystemV ABI) */
    *output << "    pushq %rbp\n";
    *output << "    movq %rsp, %rbp\n";
    
    /* Generate body */
    if (node.body) {
        node.body->accept(*this);
    }
}

void AMD64CodeGenerator::visit(TranslationUnit& node) {
    /* Emit assembly directives */
    *output << "    .text\n";
    
    /* Generate all functions */
    for (auto& func : node.functions) {
        func->accept(*this);
    }
}

#endif /* ENABLE_AMD64_BACKEND */
