#pragma once
#include <string>
#include <variant>
#include <vector>

namespace ZIR
{
struct Operand
{
    typedef std::variant<std::string, int> Value;

    const Value value;

    Operand(const Value &value);
};

// A three-address ZIR instruction
struct Instruction
{
    enum Operation
    {
        UNARY,

        // Binary operations
        ADD,
        SUBTRACT,
    };

    Operation operation;
    Operand result;
    std::vector<Operand> operands;

    Instruction &operator+=(const Operand &operand);
};
} // namespace ZIR
