#pragma once
#include <optional>
#include <string>
#include <variant>

namespace ZIR
{
struct Operand
{
    enum AddressingMode
    {
        IMMEDIATE,
        DIRECT,
    };
    
    std::variant<int, std::string> value;
    AddressingMode mode;
};

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

    Operation operation;
    std::optional<Operand> operand;
};
} // namespace ZIR
