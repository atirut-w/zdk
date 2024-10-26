#pragma once
#include "instruction.hpp"
#include <memory>
#include <string>
#include <vector>

namespace ZIR {

struct Module {
  struct Symbol {
    std::string name;
    virtual ~Symbol() = default;
  };

  struct Function : public Symbol {
    std::vector<Instruction> instructions;
    // std::vector<std::unique_ptr<Symbol>> locals;

    Function &operator+=(const Instruction &instruction);
  };

//   std::vector<std::unique_ptr<Symbol>> symbols;
  std::vector<Function> functions;
};

} // namespace ZIR
