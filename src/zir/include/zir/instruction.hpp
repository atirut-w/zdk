#pragma once
#include <optional>
#include <string>
#include <variant>

namespace ZIR
{
typedef std::variant<int, std::string> Operand;

struct Instruction
{
    enum Operation
    {
        // Basic operations
        LOAD,
        STORE,
        RETURN,

        // Unary operations
        NEGATE,
        COMPLEMENT,

        // Binary operations
        ADD,
        SUBTRACT,
    };

    enum AddressingMode
    {
        IMMEDIATE,
        DIRECT,
    };

    Operation operation;
    AddressingMode addressing_mode;
    std::optional<Operand> operand;
};
} // namespace ZIR
