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

vector<int> GPR16 = {R16_BC, R16_DE, R16_HL};

int Codegen::ralloc() {
  for (int reg : GPR16) {
    if (!(fctx.used_regs & reg)) {
      fctx.used_regs |= reg;
      return reg;
    }
  }
  return 0;
}

int Codegen::ralloc(int regs) {
  if (fctx.used_regs & regs) {
    return 0;
  }
  fctx.used_regs |= regs;
  return regs;
}

// int Codegen::sralloc() {
//   int reg = ralloc();
//   if (!reg) {
//     throw runtime_error("out of registers");
//   }
//   return reg;
// }

// int Codegen::sralloc(int reg) {
//   if (used_regs & reg) {
//     throw runtime_error("register already in use");
//   }
//   return ralloc(reg);
// }

void Codegen::rcpy(int dst, int src) {
  string dstname = reg_names[dst];
  string srcname = reg_names[src];

  if (dst == src) {
    return;
  }

  for (int i = 0; i < 2; i++) {
    os << "\tld " << dstname[1 - i] << ", " << srcname[1 - i] << "\n";
  }
}

bool Codegen::rused(int regs) { return fctx.used_regs & regs; }

void Codegen::rfree(int regs) { fctx.used_regs &= ~regs; }

void Codegen::rsave(int regs) {
  for (int reg : GPR16) {
    if (regs & reg) {
      os << "\tpush " << reg_names[reg] << "\n";
    }
  }
}

void Codegen::rrestore(int regs) {
  for (int reg : GPR16) {
    if (regs & reg) {
      os << "\tpop " << reg_names[reg] << "\n";
    }
  }
}

int Codegen::new_label() { return fctx.label++; }

void Codegen::visit(const TranslationUnit &node) {
  for (const auto &decl : node.declarations) {
    if (auto *fd = dynamic_cast<const FunctionDefinition *>(decl.get())) {
      visit(*fd);
    } else if (auto *gd = dynamic_cast<const GlobalDeclaration *>(decl.get())) {
      visit(*gd);
    } else {
      throw runtime_error("unhandled external declaration type");
    }
  }
}

void Codegen::visit(const FunctionDefinition &node) {
  fctx = {};

  os << "\t.section .text" << "\n";
  os << node.name << ":" << "\n";
  for (const auto &stmt : node.body) {
    if (auto *rs = dynamic_cast<const ReturnStatement *>(stmt.get())) {
      visit(*rs);
    } else if (auto *es = dynamic_cast<const ExpressionStatement *>(stmt.get())) {
      visit(*es);
    } else {
      throw runtime_error("unhandled statement type");
    }
  }
}

void Codegen::visit(const GlobalDeclaration &node) {
  os << "\t.section .bss" << "\n";
  os << node.name << ":" << "\n";
  os << "\t.skip 2\n";

  symbols[node.name] = Symbol{};
}

void Codegen::visit(const ReturnStatement &node) {
  if (node.expression) {
    visit(*node.expression, R16_HL);
  }
  os << "\tret" << "\n";
}

void Codegen::visit(const ExpressionStatement &node) {
  visit(*node.expression, R16_HL);
}

void Codegen::visit(const Expression &node, int reg) {
  if (auto *ic = dynamic_cast<const IntegerConstant *>(&node)) {
    visit(*ic, reg);
  } else if (auto *be = dynamic_cast<const BinaryExpression *>(&node)) {
    visit(*be, reg);
  } else if (auto *re = dynamic_cast<const RelationalExpression *>(&node)) {
    visit(*re, reg);
  } else if (auto *id = dynamic_cast<const Identifier *>(&node)) {
    visit(*id, reg);
  } else if (auto *as = dynamic_cast<const Assignment *>(&node)) {
    visit(*as, reg);
  } else {
    throw runtime_error("unhandled expression type");
  }
}

void Codegen::visit(const IntegerConstant &node, int reg) {
  os << "\tld " << reg_names[reg] << ", " << node.value << "\n";
}

void Codegen::visit(const BinaryExpression &node, int reg) {
  int lhs, rhs;
  bool restore = false;

  if (reg == R16_HL) {
    lhs = reg;
    ralloc(lhs);
  } else {
    lhs = ralloc(R16_HL);
    if (!lhs) {
      lhs = R16_HL;
      os << "\tpush hl\n";
      restore = true;
    }
  }
  visit(*node.left, lhs);

  rhs = ralloc();
  visit(*node.right, rhs);

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
    rsave(fctx.used_regs & ~(lhs | rhs | reg));
    os << "\tpush " << reg_names[rhs] << "\n";
    os << "\tpush " << reg_names[lhs] << "\n";

    // Freeing RHS early helps with stack cleanup
    rfree(rhs);
    
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

    // TODO: Improve stack cleanup
    int dump = ralloc();
    if (dump) {
      os << "\tpop " << reg_names[dump] << "\n";
      os << "\tpop " << reg_names[dump] << "\n";
      rfree(dump);
    } else {
      os << "\tinc sp\n";
      os << "\tinc sp\n";
      os << "\tinc sp\n";
      os << "\tinc sp\n";
    }

    rrestore(fctx.used_regs & ~(lhs | rhs | reg));
  }

  rcpy(reg, lhs);
  rfree(rhs);
  if (restore) {
    os << "\tpop hl\n";
  }
}

void Codegen::visit(const RelationalExpression &node, int reg) {
  int lhs, rhs;
  bool restore = false;

  if (reg == R16_HL) {
    lhs = reg;
    ralloc(lhs);
  } else {
    lhs = ralloc(R16_HL);
    if (!lhs) {
      lhs = R16_HL;
      os << "\tpush hl\n";
      restore = true;
    }
  }
  visit(*node.left, lhs);

  rhs = ralloc();
  visit(*node.right, rhs);

  os << "\tor a\n";
  os << "\tsbc " << reg_names[lhs] << ", " << reg_names[rhs] << "\n";

  // Set 0...
  os << "\tld " << reg_names[reg] << ", 0\n";
  int skip = new_label();

  // ...if not flag
  switch (node.op) {
  default:
    throw runtime_error("unhandled relational operator");
  case RelationalExpression::Eq:
    os << "\tjr nz, " << skip << "f\n";
    break;
  case RelationalExpression::Ne:
    os << "\tjr z, " << skip << "f\n";
    break;
  case RelationalExpression::Lt:
    os << "\tjr nc, " << skip << "f\n";
    break;
  case RelationalExpression::Le: {
    int run = new_label();
    os << "\tjr z, " << run << "f\n";
    os << "\tjr nc, " << skip << "f\n";
    os << run << ":\n";
    break;
  }
  case RelationalExpression::Gt:
    os << "\tjr c, " << skip << "f\n";
    os << "\tjr z, " << skip << "f\n";
    break;
  case RelationalExpression::Ge:
    os << "\tjr c, " << skip << "f\n";
    break;
  }

  // Otherwise, set 1
  os << "\tld " << reg_names[reg] << ", 1\n";
  os << skip << ":\n";

  rfree(rhs);
  if (restore) {
    os << "\tpop hl\n";
  }
}

void Codegen::visit(const Identifier &node, int reg) {
  auto it = symbols.find(node.name);
  if (it == symbols.end()) {
    throw runtime_error("undeclared identifier");
  }
  os << "\tld " << reg_names[reg] << ", (" << it->first << ")\n";
}

void Codegen::visit(const Assignment &node, int reg) {
  if (auto *id = dynamic_cast<const Identifier *>(node.lvalue.get())) {
    visit(*node.rvalue, reg);
    
    auto it = symbols.find(id->name);
    if (it == symbols.end()) {
      throw runtime_error("undeclared identifier");
    }

    os << "\tld (" << it->first << "), " << reg_names[reg] << "\n";
  } else {
    throw runtime_error("unhandled assignment type");
  }
}
