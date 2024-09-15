#pragma once
#include "instruction.hpp"
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
    };
};
} // namespace ZIR
