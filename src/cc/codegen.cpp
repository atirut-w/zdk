#include "codegen.hpp"
#include "ast.hpp"
#include <cassert>
#include <iostream>
#include <map>
#include <string>

using namespace std;

enum R8 {
  R8_A = 1 << 6,
  R8_B = 1 << 5,
  R8_C = 1 << 4,
  R8_D = 1 << 3,
  R8_E = 1 << 2,
  R8_H = 1 << 1,
  R8_L = 1 << 0,
};

enum R16 {
  R16_BC = R8_B | R8_C,
  R16_DE = R8_D | R8_E,
  R16_HL = R8_H | R8_L,
};

map<int, string> reg_names = {
    {R8_A, "a"},    {R8_B, "b"},    {R8_C, "c"},    {R8_D, "d"}, {R8_E, "e"}, {R8_H, "h"}, {R8_L, "l"},

    {R16_BC, "bc"}, {R16_DE, "de"}, {R16_HL, "hl"},
};

int Codegen::ralloc() {
  for (int reg : {R16_BC, R16_DE, R16_HL}) {
    if (!(used_regs & reg)) {
      used_regs |= reg;
      return reg;
    }
  }
  throw runtime_error("out of registers");
}

int Codegen::ralloc(int reg) {
  if (used_regs & reg) {
    // throw runtime_error("register already allocated");
    return 0;
  }
  used_regs |= reg;
  return reg;
}

void Codegen::rfree(int reg) { used_regs &= ~reg; }

void Codegen::visit(const TranslationUnit &node) {
  for (const auto &decl : node.declarations) {
    if (auto *fd = dynamic_cast<const FunctionDefinition *>(decl.get())) {
      visit(*fd);
    }
  }
}

void Codegen::visit(const FunctionDefinition &node) {
  os << node.name << ":" << "\n";
  for (const auto &stmt : node.body) {
    if (auto *rs = dynamic_cast<const ReturnStatement *>(stmt.get())) {
      visit(*rs);
    } else {
      throw runtime_error("unhandled statement type");
    }
  }
}

void Codegen::visit(const ReturnStatement &node) {
  if (node.expression) {
    visit(*node.expression);
  }
  os << "\tret" << "\n";
}

int Codegen::visit(const Expression &node, int reg) {
  if (auto *ic = dynamic_cast<const IntegerConstant *>(&node)) {
    return visit(*ic, reg);
  } else if (auto *be = dynamic_cast<const BinaryExpression *>(&node)) {
    return visit(*be, reg);
  } else {
    throw runtime_error("unhandled expression type");
  }
}

int Codegen::visit(const IntegerConstant &node, int reg) {
  if (!reg) {
    reg = ralloc();
  } else {
    reg = ralloc(reg);
  }
  os << "\tld " << reg_names[reg] << ", " << node.value << "\n";
  return reg;
}

int Codegen::visit(const BinaryExpression &node, int reg) {
  int lhs = visit(*node.left, R16_HL);
  int rhs = visit(*node.right);

  switch (node.op) {
  case BinaryExpression::Add:
    os << "\tadd " << reg_names[lhs] << ", " << reg_names[rhs] << "\n";
    break;
  case BinaryExpression::Sub:
    os << "\tor a\n";
    os << "\tsbc " << reg_names[lhs] << ", " << reg_names[rhs] << "\n";
    break;
  case BinaryExpression::Mul:
  case BinaryExpression::Div:
  case BinaryExpression::Mod:
    os << "\tpush " << reg_names[rhs] << "\n";
    os << "\tpush " << reg_names[lhs] << "\n";
    
    string routine;
    switch (node.op) {
    case BinaryExpression::Mul:
      routine = "__mulsi3";
      break;
    case BinaryExpression::Div:
      routine = "__divsi3";
      break;
    case BinaryExpression::Mod:
      routine = "__modsi3";
      break;
    }
    os << "\tcall " << routine << "\n";

    // TODO: Not use a register for stack cleanup
    int dump = ralloc();
    os << "\tpop " << reg_names[dump] << "\n";
    os << "\tpop " << reg_names[dump] << "\n";
    rfree(dump);
  }

  rfree(rhs);
  return lhs;
}
