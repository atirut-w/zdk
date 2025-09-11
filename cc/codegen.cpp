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

void Codegen::visit(FunctionDefinition &node) {
  next_label = 0;
  add_global(node.name, Symbol{});

  os << "\t.section .text" << "\n";
  os << node.name << ":" << "\n";
  os << "\tpush ix\n";
  os << "\tld ix, 0\n";
  os << "\tadd ix, sp\n";

  for (auto &stmt : node.body) {
    if (stmt) {
      visit(*stmt);
    }
  }
}

void Codegen::visit(GlobalDeclaration &node) {
  add_global(node.name, Symbol{});

  os << "\t.section .bss" << "\n";
  os << node.name << ":" << "\n";
  os << "\t.skip 2\n";
}

void Codegen::visit(ReturnStatement &node) {
  if (node.expression) {
    visit(*node.expression);
  }
  os << "\tld sp, ix\n";
  os << "\tpop ix\n";
  os << "\tret" << "\n";
}

void Codegen::visit(ExpressionStatement &node) { visit(*node.expression); }

void Codegen::visit(IfStatement &node) {
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

void Codegen::visit(WhileStatement &node) {
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

void Codegen::visit(ForStatement &node) {
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

void Codegen::visit(IntegerConstant &node) {
  if (node.rhs) {
    os << "\tld de, " << node.value << "\n";
  } else {
    os << "\tld hl, " << node.value << "\n";
  }
}

void Codegen::visit(BinaryExpression &node) {
  visit(*node.left);

  if (node.right->clobbers_hl) {
    os << "\tpush hl\n";
  }
  visit(*node.right);
  if (node.right->clobbers_hl) {
    os << "\tpop hl\n";
  }

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

  if (node.rhs) {
    os << "\tld e, l\n";
    os << "\tld d, h\n";
  }
}

void Codegen::visit(RelationalExpression &node) {
  visit(*node.left);

  if (node.right->clobbers_hl) {
    os << "\tpush hl\n";
  }
  visit(*node.right);
  if (node.right->clobbers_hl) {
    os << "\tpop hl\n";
  }

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

  if (node.rhs) {
    os << "\tld e, l\n";
    os << "\tld d, h\n";
  }
}

void Codegen::visit(IdentifierExpression &node) {
  auto it = symbols.find(node.name);
  if (it == symbols.end()) {
    throw runtime_error("undeclared identifier");
  }
  if (node.rhs) {
    os << "\tld de, (" << it->first << ")\n";
  } else {
    os << "\tld hl, (" << it->first << ")\n";
  }
}

void Codegen::visit(Assignment &node) {
  if (auto *ie = dynamic_cast<IdentifierExpression *>(node.lvalue.get())) {
    visit(*node.rvalue);

    auto it = symbols.find(ie->name);
    if (it == symbols.end()) {
      throw runtime_error("undeclared identifier");
    }

    if (node.rhs) {
      os << "\tld (" << it->first << "), de\n";
    } else {
      os << "\tld (" << it->first << "), hl\n";
    }
  } else {
    throw runtime_error("unhandled assignment type");
  }
}
