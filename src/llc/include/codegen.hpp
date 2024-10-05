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
    Allocator allocator;
    std::vector<LivenessAnalyzer::Interval> intervals;
    std::map<llvm::Value *, int> locals;
    std::map<llvm::Value *, int> loaded;
    int stack_size = 0;
  } ctx;

  void pregen_function(llvm::Function &func);
  void write_instruction(llvm::Instruction &inst);
  void load(llvm::Value *val);
  void vacate(uint8_t regs);
  void assert_regs(uint8_t regs);

  // void generate_function(llvm::Function &func);
  // void generate_instruction(llvm::Instruction &inst);
  void generate_return(llvm::ReturnInst *ret);
  void generate_epilogue();
  void generate_load(llvm::LoadInst *load);
  void generate_store(llvm::StoreInst *store);
  void generate_sext(llvm::SExtInst *sext);

public:
  Codegen(std::ostream &os);

  std::any visit_module(llvm::Module &module) override;
  std::any visit_function(llvm::Function &function) override;
  std::any visit_instruction(llvm::Instruction &inst) override;
};
