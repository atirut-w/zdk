#pragma once
#include "ir_visitor.hpp"
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <map>
#include <ostream>
#include <string>

class Codegen : public IRVisitor {
  std::ostream &os;
  llvm::Module *module;

  struct {
    std::map<llvm::Value *, int> offsets;
  } ctx;

  void generate_prologue(llvm::Function &function);
  void comment_instruction(llvm::Instruction &inst);
  std::string get_ix_offset(llvm::Value *value, int offset = 0);

  void visit_return(llvm::ReturnInst *inst);
  void visit_store(llvm::StoreInst *inst);

public:
  Codegen(std::ostream &os);

  virtual std::any visit_module(llvm::Module &module) override;
  virtual std::any visit_function(llvm::Function &function) override;
  virtual std::any visit_instruction(llvm::Instruction &inst) override;
};
