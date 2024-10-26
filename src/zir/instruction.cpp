#include <stdexcept>
#include <zir/instruction.hpp>

using namespace std;
using namespace ZIR;

Instruction::Instruction(Operation operation) : operation(operation) {}

Instruction::Instruction(Operation operation, const Value &result)
    : operation(operation), result(result) {}

Instruction &Instruction::operator+=(const Value &operand) {
  operands.push_back(operand);
  return *this;
}
