#include "ir_visitor.hpp"
#include <any>

using namespace std;

void IRVisitor::visit(llvm::Module &module) {
  for (auto &func : module.functions()) {
    visit(func);
  }
}

void IRVisitor::visit(llvm::Function &function) {
  for (auto &block : function) {
    visit(block);
  }
}

void IRVisitor::visit(llvm::BasicBlock &block) {
  for (auto &inst : block) {
    visit(inst);
  }
}

void IRVisitor::visit(llvm::Instruction &inst) {}
