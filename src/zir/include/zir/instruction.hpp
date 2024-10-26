#pragma once
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace ZIR {

typedef std::variant<std::string, char, int> Value;

// A three-address ZIR instruction
struct Instruction {
  enum Operation {
    // Return a value.
    RETURN,

    // Perform unary operation.
    UNARY,
    // Perform binary operation.
    BINARY,
  };

  Operation operation;
  std::optional<Value> result;
  std::vector<Value> operands;

  Instruction(Operation operation);
  Instruction(Operation operation, const Value &result);
  Instruction &operator+=(const Value &operand);
};
} // namespace ZIR
