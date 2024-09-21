#include <stdexcept>
#include <zir/instruction.hpp>

using namespace std;
using namespace ZIR;

Operand::Operand(const Value &value) : value(value)
{
}

Instruction::Instruction(Operation operation) : operation(operation)
{
}

Instruction::Instruction(Operation operation, const Operand &result) : operation(operation), result(result)
{
}

Instruction &Instruction::operator+=(const Operand &operand)
{
    if (operands.size() == 2)
        throw runtime_error("too many operands");
    operands.push_back(operand);
    return *this;
}
