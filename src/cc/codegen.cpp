#include "codegen.hpp"
#include "ast.hpp"

using namespace std;

void Codegen::load_imm(int value) {
  os << "\tld hl, " << value << endl;
}

void Codegen::visit(const TranslationUnit &node) {
  for (const auto &decl : node.declarations) {
    if (auto fd = dynamic_cast<const FunctionDefinition *>(decl.get())) {
      visit(*fd);
    }
  }
}

void Codegen::visit(const FunctionDefinition &node) {
  os << node.name << ":" << endl;
  for (const auto &stmt : node.body) {
    if (auto rs = dynamic_cast<const ReturnStatement *>(stmt.get())) {
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
  os << "\tret" << endl;
}

void Codegen::visit(const Expression &node) {
  if (auto ic = dynamic_cast<const IntegerConstant *>(&node)) {
    visit(*ic);
  } else {
    throw runtime_error("unhandled expression type");
  }
}

void Codegen::visit(const IntegerConstant &node) {
  load_imm(node.value);
}
