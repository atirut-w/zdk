#include "codegen.hpp"
#include "ast.hpp"
#include <cassert>
#include <iostream>
#include <map>
#include <string>

using namespace std;

int Codegen::new_label() { return next_label++; }

void Codegen::add_global(const string &name, const Symbol &symbol) {
  if (symbols.find(name) != symbols.end()) {
    throw runtime_error("duplicate symbol");
  }
  symbols[name] = symbol;
}

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
  next_label = 0;
  expr_state.hl_free = true;
  expr_state.de_free = true;
  add_global(node.name, Symbol{});

  os << "\t.section .text" << "\n";
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
  add_global(node.name, Symbol{});

  os << "\t.section .bss" << "\n";
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
    visit(*node.expression);
  }
  os << "\tld sp, ix\n";
  os << "\tpop ix\n";
  os << "\tret" << "\n";
}

void Codegen::visit(const ExpressionStatement &node) { visit(*node.expression); }

void Codegen::visit(const IfStatement &node) {
  visit(*node.condition);

  int else_label = new_label();
  int end_label = new_label();

  // TODO: Handle different types
  os << "\tld a, l\n";
  os << "\tor h\n";
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
  int loop_label = new_label();
  int skip_label = new_label();

  os << loop_label << ":\n";
  visit(*node.condition);

  // TODO: Handle different types
  os << "\tld a, l\n";
  os << "\tor h\n";
  os << "\tjr z, " << skip_label << "f\n";

  visit(*node.body);
  os << "\tjp " << loop_label << "b\n";

  os << skip_label << ":\n";
}

void Codegen::visit(const ForStatement &node) {
  if (node.init) {
    visit(*node.init);
  }

  int loop_label = new_label();
  int skip_label = new_label();

  os << loop_label << ":\n";

  if (node.condition) {
    visit(*node.condition);

    // TODO: Handle different types
    os << "\tld a, l\n";
    os << "\tor h\n";
    os << "\tjr z, " << skip_label << "f\n";
  }
  if (node.body) {
    visit(*node.body);
  }
  if (node.update) {
    visit(*node.update);
  }

  os << "\tjp " << loop_label << "b\n";
  os << skip_label << ":\n";
}

void Codegen::visit(const Expression &node, bool rhs) {
  expr_stack.push_back(expr_state);

  if (auto *ic = dynamic_cast<const IntegerConstant *>(&node)) {
    visit(*ic, rhs);
  } else if (auto *be = dynamic_cast<const BinaryExpression *>(&node)) {
    visit(*be, rhs);
  } else if (auto *re = dynamic_cast<const RelationalExpression *>(&node)) {
    visit(*re, rhs);
  } else if (auto *ie = dynamic_cast<const IdentifierExpression *>(&node)) {
    visit(*ie, rhs);
  } else if (auto *as = dynamic_cast<const Assignment *>(&node)) {
    visit(*as, rhs);
  } else {
    throw runtime_error("unhandled expression type");
  }

  if (!expr_stack.empty()) {
    expr_state = expr_stack.back();
    expr_stack.pop_back();
  }
}

void Codegen::visit(const IntegerConstant &node, bool rhs) {
  if (rhs) {
    os << "\tld de, " << node.value << "\n";
  } else {
    os << "\tld hl, " << node.value << "\n";
  }
}

void Codegen::visit(const BinaryExpression &node, bool rhs) {
  bool pushed_hl = !expr_state.hl_free;
  if (pushed_hl) {
    os << "\tpush hl\n";
  }
  visit(*node.left);
  expr_state.hl_free = false;

  bool pushed_de = !expr_state.de_free;
  if (pushed_de) {
    os << "\tpush de\n";
  }
  visit(*node.right, true);
  expr_state.de_free = false;

  switch (node.op) {
    default:
      throw runtime_error("unhandled binary operator");
    case BinaryExpression::Add:
      os << "\tadd hl, de\n";
      break;
    case BinaryExpression::Sub:
      os << "\tor a\n";
      os << "\tsbc hl, de\n";
      break;
    case BinaryExpression::Mul:
    case BinaryExpression::Div:
    case BinaryExpression::Mod:
      os << "\tpush de\n";
      os << "\tpush hl\n";

      switch (node.op) {
        default:
          throw runtime_error("unhandled binary operator");
        case BinaryExpression::Mul:
          os << "\tcall __mulsi3\n";
          break;
        case BinaryExpression::Div:
          os << "\tcall __divsi3\n";
          break;
        case BinaryExpression::Mod:
          os << "\tcall __modsi3\n";
          break;
      }

      os << "\tpop bc\n";
      os << "\tpop bc\n";
      break;
  }

  if (rhs) {
    os << "\tld e, l\n";
    os << "\tld d, h\n";
  }

  if (pushed_de) {
    os << "\tpop de\n";
    expr_state.de_free = true;
  }
  if (pushed_hl) {
    os << "\tpop hl\n";
    expr_state.hl_free = true;
  }
}

void Codegen::visit(const RelationalExpression &node, bool rhs) {
  bool pushed_hl = !expr_state.hl_free;
  if (pushed_hl) {
    os << "\tpush hl\n";
  }
  visit(*node.left);
  expr_state.hl_free = false;

  bool pushed_de = !expr_state.de_free;
  if (pushed_de) {
    os << "\tpush de\n";
  }
  visit(*node.right, true);
  expr_state.de_free = false;

  os << "\tor a\n";
  os << "\tsbc hl, de\n";

  // Set 0...
  os << "\tld hl, 0\n";
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
  os << "\tld hl, 1\n";
  os << skip << ":\n";

  if (rhs) {
    os << "\tld e, l\n";
    os << "\tld d, h\n";
  }

  if (pushed_de) {
    os << "\tpop de\n";
    expr_state.de_free = true;
  }
  if (pushed_hl) {
    os << "\tpop hl\n";
    expr_state.hl_free = true;
  }
}

void Codegen::visit(const IdentifierExpression &node, bool rhs) {
  auto it = symbols.find(node.name);
  if (it == symbols.end()) {
    throw runtime_error("undeclared identifier");
  }
  if (rhs) {
    os << "\tld de, (" << it->first << ")\n";
  } else {
    os << "\tld hl, (" << it->first << ")\n";
  }
}

void Codegen::visit(const Assignment &node, bool rhs) {
  if (auto *ie = dynamic_cast<const IdentifierExpression *>(node.lvalue.get())) {
    visit(*node.rvalue, rhs);

    auto it = symbols.find(ie->name);
    if (it == symbols.end()) {
      throw runtime_error("undeclared identifier");
    }

    if (rhs) {
      os << "\tld (" << it->first << "), de\n";
    } else {
      os << "\tld (" << it->first << "), hl\n";
    }
  } else {
    throw runtime_error("unhandled assignment type");
  }
}
