#pragma once
#include "zir/instruction.hpp"
#include "zir/module.hpp"
#include <map>
#include <ostream>
#include <string>

class Codegen
{
    std::ostream &out;
    const ZIR::Module &module;

public:
    Codegen(std::ostream &out, const ZIR::Module &module);
    struct
    {
        const ZIR::Module::Function *function;
        std::map<std::string, int> offsets;
    } ctx;

    void load(const ZIR::Operand &operand);
    void store(const ZIR::Operand &operand);

    void generate();
    void generate_function(const ZIR::Module::Function &function);
    void generate_epilogue();
    void generate_instruction(const ZIR::Instruction &instruction);
};
