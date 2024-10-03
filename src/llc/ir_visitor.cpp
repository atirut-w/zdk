#include "ir_visitor.hpp"
#include <any>

using namespace std;

std::any IRVisitor::visit_module(llvm::Module &module) {
  for (auto &func : module.functions()) {
    visit_function(func);
  }
  return {};
}

std::any IRVisitor::visit_function(llvm::Function &function) {
  for (auto &block : function) {
    visit_block(block);
  }
  return {};
}

std::any IRVisitor::visit_block(llvm::BasicBlock &block) {
  for (auto &inst : block) {
    visit_instruction(inst);
  }
  return {};
}

std::any IRVisitor::visit_instruction(llvm::Instruction &inst) { return {}; }
