#pragma once
#include "allocator.hpp"
#include "ir_visitor.hpp"
#include "liveness_analyzer.hpp"
#include <cstdint>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <map>
#include <ostream>
#include <vector>

class Codegen : public IRVisitor {
  std::ostream &os;
  llvm::Module *module;

  struct {
    std::vector<LivenessAnalyzer::Interval> intervals;
    std::map<llvm::Value *, int> stack_offsets;
    int stack_size = 0;
    int pos = 0;
  } ctx;

  void compute_offsets(llvm::Function &func);

public:
  Codegen(std::ostream &os);

  std::any visit_module(llvm::Module &module) override;
  std::any visit_function(llvm::Function &function) override;
};
