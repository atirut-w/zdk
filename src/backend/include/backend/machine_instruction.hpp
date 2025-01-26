#pragma once
#include "backend/machine_operand.hpp"
#include <string>
#include <vector>

struct MachineInstruction {
  int opcode;
  std::string mnemonic;
  std::vector<MachineOperand> operands;
};
