#include <stdexcept>
#include <zir/instruction.hpp>

using namespace std;
using namespace ZIR;

Instruction &Instruction::operator+=(const Operand &operand)
{
    if (operands.size() == 2)
        throw runtime_error("too many operands");
    operands.push_back(operand);
    return *this;
}