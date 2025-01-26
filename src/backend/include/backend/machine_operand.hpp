#pragma once

class MachineInstruction;

class MachineOperand {
public:
  enum Kind {
    Register,
    Immediate,
    Result,
  };

  Kind kind;
  union {
    int reg;
    int imm;
    MachineInstruction *result;
  };
  int offset;
  bool is_pointer;
};
