#include "codegen.hpp"

#ifdef ENABLE_Z80_BACKEND

Z80CodeGenerator::Z80CodeGenerator() : output(nullptr), lastValue(0) {}

void Z80CodeGenerator::generate(TranslationUnit& tu, std::ostream& out) {
    output = &out;
    tu.accept(*this);
}

void Z80CodeGenerator::visit(IntegerLiteral& node) {
    /* Store value for later use */
    lastValue = node.value;
}

void Z80CodeGenerator::visit(ReturnStatement& node) {
    /* Evaluate the return expression */
    if (node.expr) {
        node.expr->accept(*this);
        /* Move 16-bit value to HL register pair */
        *output << "    ld hl, " << lastValue << "\n";
    }
    
    /* Function epilogue - restore IX and return */
    *output << "    ld sp, ix\n";
    *output << "    pop ix\n";
    *output << "    ret\n";
}

void Z80CodeGenerator::visit(CompoundStatement& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
}

void Z80CodeGenerator::visit(FunctionDeclaration& node) {
    /* Emit function label */
    *output << "    .globl " << node.name << "\n";
    *output << node.name << ":\n";
    
    /* Function prologue - IX as frame pointer */
    *output << "    push ix\n";
    *output << "    ld ix, 0\n";
    *output << "    add ix, sp\n";
    
    /* Generate body */
    if (node.body) {
        node.body->accept(*this);
    }
}

void Z80CodeGenerator::visit(TranslationUnit& node) {
    /* Emit assembly directive for code section */
    *output << "    .text\n";
    
    /* Generate all functions */
    for (auto& func : node.functions) {
        func->accept(*this);
    }
}

#endif /* ENABLE_Z80_BACKEND */
