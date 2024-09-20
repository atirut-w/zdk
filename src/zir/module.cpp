#include <zir/module.hpp>

using namespace ZIR;

Module::Function &Module::Function::operator+=(const Instruction &instruction)
{
    instructions.push_back(instruction);
    return *this;
}
