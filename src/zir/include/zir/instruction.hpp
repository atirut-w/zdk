#pragma once
#include <optional>
#include <string>
#include <variant>

namespace ZIR
{
struct Instruction
{
    typedef std::variant<int, std::string> Operand;

    enum Operation
    {
        // Basic operations
        LOAD,
        RETURN,
    };

    enum AddressingMode
    {
        IMMEDIATE,
    };

    Operation operation;
    AddressingMode addressing_mode;
    std::optional<Operand> operand;
};
} // namespace ZIR
