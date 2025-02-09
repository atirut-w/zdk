#include "codegen.hpp"
#include "ast.hpp"
#include "type.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <string>

using namespace std;

enum R8 {
  R8_A = 1 << 0,
  R8_F = 1 << 1,
  R8_B = 1 << 2,
  R8_C = 1 << 3,
  R8_D = 1 << 4,
  R8_E = 1 << 5,
  R8_H = 1 << 6,
  R8_L = 1 << 7,
};

enum R16 {
  R16_AF = R8_A | R8_F,
  R16_BC = R8_B | R8_C,
  R16_DE = R8_D | R8_E,
  R16_HL = R8_H | R8_L,
};

map<int, string> reg_names = {
    {R8_A, "a"},    {R8_B, "b"},    {R8_C, "c"},    {R8_D, "d"},    {R8_E, "e"}, {R8_H, "h"}, {R8_L, "l"},

    {R16_AF, "af"}, {R16_BC, "bc"}, {R16_DE, "de"}, {R16_HL, "hl"},
};

vector<int> GPR8 = {R8_A, R8_B, R8_C, R8_D, R8_E, R8_H, R8_L};
vector<int> GPR16 = {R16_BC, R16_DE, R16_HL};

int Codegen::ralloc(Type type) {
  switch (type) {
  default:
    throw runtime_error("unhandled type");
  case Type::Char:
    for (int reg : GPR8) {
      if (!(fctx.used_regs & reg)) {
        fctx.used_regs |= reg;
        return reg;
      }
    }
    break;
  case Type::Int:
    for (int reg : GPR16) {
      if (!(fctx.used_regs & reg)) {
        fctx.used_regs |= reg;
        return reg;
      }
    }
    break;
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

  for (int i = 0; i < std::min(dstname.size(), srcname.size()); i++) {
    os << "\tld " << dstname[i] << ", " << srcname[i] << "\n";
  }

  for (int i = std::min(dstname.size(), srcname.size()); i < dstname.size(); i++) {
    os << "\tld " << dstname[i] << ", 0\n";
  }
}

bool Codegen::rused(int regs) { return fctx.used_regs & regs; }

void Codegen::rfree(int regs) { fctx.used_regs &= ~regs; }

void Codegen::rsave(int regs) {
  for (int reg : {R16_AF, R16_BC, R16_DE, R16_HL}) {
    if (regs & reg) {
      os << "\tpush " << reg_names[reg] << "\n";
    }
  }
}

void Codegen::rrestore(int regs) {
  vector<int> list = {R16_AF, R16_BC, R16_DE, R16_HL};
  for (int i = list.size() - 1; i >= 0; i--) {
    int reg = list[i];
    if (regs & reg) {
      os << "\tpop " << reg_names[reg] << "\n";
    }
  }
}

int Codegen::rabi(Type type) {
  switch (type) {
  default:
    throw runtime_error("unhandled type");
  case Type::Char:
    return R8_A;
  case Type::Int:
    return R16_HL;
  }
}

int Codegen::new_label() { return fctx.label++; }

// void Codegen::add_global(const string &name, const Symbol &symbol) {
//   if (symtab.gsym.find(name) != symtab.gsym.end()) {
//     throw runtime_error("duplicate symbol");
//   }
//   symtab.gsym[name] = symbol;
// }

void Codegen::visit(const TranslationUnit &node) {
  vector<const GlobalDeclaration *> globals;

  for (const auto &decl : node.declarations) {
    if (auto *fd = dynamic_cast<const FunctionDefinition *>(decl.get())) {
      visit(*fd);
    } else if (auto *gd = dynamic_cast<const GlobalDeclaration *>(decl.get())) {
      globals.push_back(gd);
    } else {
      throw runtime_error("unhandled external declaration type");
    }
  }

  os << "\t.section .bss\n";
  for (const auto &gd : globals) {
    visit(*gd);
  }
}

void Codegen::visit(const FunctionDefinition &node) {
  fctx = {};
  // add_global(node.name, Symbol{.kind = Symbol::Function});

  os << node.name << ":" << "\n";
  os << "\tpush ix\n";
  os << "\tld ix, 0\n";
  os << "\tadd ix, sp\n";

  for (const auto &stmt : node.body) {
    if (stmt) {
      visit(*stmt);
    }
  }
}

void Codegen::visit(const GlobalDeclaration &node) {
  os << node.name << ":" << "\n";
  os << "\t.skip 2\n";
}

void Codegen::visit(const Statement &node) {
  if (auto *rs = dynamic_cast<const ReturnStatement *>(&node)) {
    visit(*rs);
  } else if (auto *es = dynamic_cast<const ExpressionStatement *>(&node)) {
    visit(*es);
  } else if (auto *is = dynamic_cast<const IfStatement *>(&node)) {
    visit(*is);
  } else if (auto *ws = dynamic_cast<const WhileStatement *>(&node)) {
    visit(*ws);
  } else if (auto *fs = dynamic_cast<const ForStatement *>(&node)) {
    visit(*fs);
  } else {
    throw runtime_error("unhandled statement type");
  }
}

void Codegen::visit(const ReturnStatement &node) {
  if (node.expression) {
    visit(*node.expression, R16_HL);
  }
  os << "\tld sp, ix\n";
  os << "\tpop ix\n";
  os << "\tret" << "\n";
}

void Codegen::visit(const ExpressionStatement &node) { visit(*node.expression, R16_HL); }

void Codegen::visit(const IfStatement &node) {
  int reg = R16_HL;
  visit(*node.condition, reg);

  int else_label = new_label();
  int end_label = new_label();

  os << "\tld a, " << reg_names[reg][0] << "\n";
  os << "\tor " << reg_names[reg][1] << "\n";
  os << "\tjr z, " << else_label << "f\n";

  visit(*node.then_statement);
  os << "\tjr " << end_label << "f\n";

  os << else_label << ":\n";
  if (node.else_statement) {
    visit(*node.else_statement);
  }

  os << end_label << ":\n";
}

void Codegen::visit(const WhileStatement &node) {
  int reg = R16_HL;
  int loop_label = new_label();
  int skip_label = new_label();

  os << loop_label << ":\n";
  visit(*node.condition, reg);

  os << "\tld a, " << reg_names[reg][0] << "\n";
  os << "\tor " << reg_names[reg][1] << "\n";
  os << "\tjr z, " << skip_label << "f\n";

  visit(*node.body);
  os << "\tjp " << loop_label << "b\n";

  os << skip_label << ":\n";
}

void Codegen::visit(const ForStatement &node) {
  int reg = R16_HL;

  if (node.init) {
    visit(*node.init, reg);
  }

  int loop_label = new_label();
  int skip_label = new_label();

  os << loop_label << ":\n";

  if (node.condition) {
    visit(*node.condition, reg);

    os << "\tld a, " << reg_names[reg][0] << "\n";
    os << "\tor " << reg_names[reg][1] << "\n";
    os << "\tjr z, " << skip_label << "f\n";
  }
  if (node.body) {
    visit(*node.body);
  }
  if (node.update) {
    visit(*node.update, reg);
  }

  os << "\tjp " << loop_label << "b\n";
  os << skip_label << ":\n";
}

void Codegen::visit(const Expression &node, int reg) {
  if (auto *ic = dynamic_cast<const IntegerConstant *>(&node)) {
    visit(*ic, reg);
  } else if (auto *be = dynamic_cast<const BinaryExpression *>(&node)) {
    visit(*be, reg);
  } else if (auto *re = dynamic_cast<const RelationalExpression *>(&node)) {
    visit(*re, reg);
  } else if (auto *ie = dynamic_cast<const IdentifierExpression *>(&node)) {
    visit(*ie, reg);
  } else if (auto *as = dynamic_cast<const Assignment *>(&node)) {
    visit(*as, reg);
  } else if (auto *ce = dynamic_cast<const CastExpression *>(&node)) {
    visit(*ce, reg);
  } else {
    throw runtime_error("unhandled expression type");
  }
}

void Codegen::visit(const IntegerConstant &node, int reg) { os << "\tld " << reg_names[reg] << ", " << node.value << "\n"; }

void Codegen::visit(const BinaryExpression &node, int reg) {
  int lhs, rhs;
  bool restore = false;

  if (reg == rabi(node.type)) {
    lhs = reg;
    ralloc(lhs);
  } else {
    lhs = ralloc(rabi(node.type));
    if (!lhs) {
      lhs = rabi(node.type);
      os << "\tpush hl\n";
      restore = true;
    }
  }
  visit(*node.left, lhs);

  rhs = ralloc(node.right->type);
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
    int dump = ralloc(Type::Int);
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

  if (reg == rabi(node.type)) {
    lhs = reg;
    ralloc(lhs);
  } else {
    lhs = ralloc(rabi(node.type));
    if (!lhs) {
      lhs = rabi(node.type);
      os << "\tpush hl\n";
      restore = true;
    }
  }
  visit(*node.left, lhs);

  rhs = ralloc(node.right->type);
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

void Codegen::visit(const IdentifierExpression &node, int reg) {
  auto it = symtab.gsym.find(node.name);
  if (it == symtab.gsym.end()) {
    throw runtime_error("undeclared identifier");
  }
  os << "\tld " << reg_names[reg] << ", (" << it->first << ")\n";
}

void Codegen::visit(const Assignment &node, int reg) {
  if (auto *ie = dynamic_cast<const IdentifierExpression *>(node.lvalue.get())) {
    visit(*node.rvalue, reg);

    auto it = symtab.gsym.find(ie->name);
    if (it == symtab.gsym.end()) {
      throw runtime_error("undeclared identifier");
    }

    os << "\tld (" << it->first << "), " << reg_names[reg] << "\n";
  } else {
    throw runtime_error("unhandled assignment type");
  }
}

void Codegen::visit(const CastExpression &node, int reg) {
  visit(*node.expression, rabi(node.expression->type));
  rcpy(reg, rabi(node.expression->type));
}
