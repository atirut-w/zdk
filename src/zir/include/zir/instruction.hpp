#pragma once
#include <string>
#include <variant>
#include <vector>

namespace ZIR
{
// A three-address ZIR instruction
struct Instruction
{
    enum Operation
    {
        // Unary operations
        NEGATE,
        COMPLEMENT,

        // Binary operations
        ADD,
        SUBTRACT,
    };

    typedef std::variant<int, std::string> Operand;

    Operation operation;
    Operand result;
    std::vector<Operand> operands;

    Instruction &operator+=(const Operand &operand);
};
} // namespace ZIR
