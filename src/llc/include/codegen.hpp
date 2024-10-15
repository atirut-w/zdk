#pragma once
#include "ir_visitor.hpp"
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <ostream>

class Codegen : public IRVisitor {
  std::ostream &os;
  llvm::Module *module;

  struct {
  } ctx;

public:
  Codegen(std::ostream &os);
};
