#pragma once
#include "zir/instruction.hpp"
#include "zir/module.hpp"
#include <ostream>

class Codegen
{
    std::ostream &out;
    const ZIR::Module &module;

public:
    Codegen(std::ostream &out, const ZIR::Module &module);

    void load(const ZIR::Operand &operand);

    void generate();
    void generate_function(const ZIR::Module::Function &function);
    void generate_instruction(const ZIR::Instruction &instruction);
};
