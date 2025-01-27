#pragma once
#include "backend/machine_instruction.hpp"

struct MachineFunction {
  std::vector<MachineInstruction> instructions;
};
