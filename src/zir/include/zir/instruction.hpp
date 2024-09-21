#pragma once
#include <optional>
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
        // Return a value
        RETURN,
        
        UNARY,
    };

    Operation operation;
    std::optional<Operand> result;
    std::vector<Operand> operands;

    Instruction(Operation operation);
    Instruction(Operation operation, const Operand &result);
    Instruction &operator+=(const Operand &operand);
};
} // namespace ZIR
