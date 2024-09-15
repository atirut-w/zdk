#pragma once
#include "instruction.hpp"
#include <memory>
#include <string>
#include <vector>

namespace ZIR
{
struct Module
{
    struct Symbol
    {
        std::string name;
        virtual ~Symbol() = default;
    };

    struct Function : public Symbol
    {
        std::vector<Instruction> instructions;
        std::vector<std::unique_ptr<Symbol>> locals;
    };

    std::vector<std::unique_ptr<Symbol>> symbols;
};
} // namespace ZIR
