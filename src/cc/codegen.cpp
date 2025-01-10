#include "codegen.hpp"
#include "ast.hpp"

using namespace std;

void Codegen::load_imm(int value) {
  os << "\tld hl, " << value << "\n";
}

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

void Codegen::visit(const Expression &node) {
  if (auto *ic = dynamic_cast<const IntegerConstant *>(&node)) {
    visit(*ic);
  } else if (auto *be = dynamic_cast<const BinaryExpression *>(&node)) {
    visit(*be);
  } else {
    throw runtime_error("unhandled expression type");
  }
}

void Codegen::visit(const IntegerConstant &node) {
  load_imm(node.value);
}

void Codegen::visit(const BinaryExpression &node) {
  visit(*node.right);
  os << "\tpush hl" << "\n";
  visit(*node.left);
  
  switch (node.op) {
  case BinaryExpression::Operator::Add:
    os << "\tpop de\n";
    os << "\tadd hl, de\n";
    break;
  case BinaryExpression::Operator::Sub:
    os << "\tpop de\n";
    os << "\tor a\n";
    os << "\tsub hl, de\n";
  case BinaryExpression::Operator::Mul:
  case BinaryExpression::Operator::Div:
  case BinaryExpression::Operator::Mod:
    os << "\tpush hl\n";

    switch (node.op) {
    case BinaryExpression::Operator::Mul:
      os << "\tcall __mulsi3\n";
      break;
    case BinaryExpression::Operator::Div:
      os << "\tcall __divsi3\n";
      break;
    case BinaryExpression::Operator::Mod:
      os << "\tcall __modsi3\n";
      break;
    }

    os << "\tpop bc\n";
    os << "\tpop bc\n";
    break;
  }
}
