#include "codegen.hpp"
#include "CParser.h"
#include <algorithm>

using namespace std;

void Codegen::load_imm(string reg, int value) {
  if (reg.length() > 2) {
    os << "\tld " << reg.substr(2, 2) << ", " << (value & 0xff) << "\n";
    os << "\tld " << reg.substr(0, 2) << ", " << (value >> 8) << "\n";
  } else {
    os << "\tld " << reg << ", " << value << "\n";
  }
}

any Codegen::visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) {
  string name = dynamic_cast<CParser::FunctionDeclaratorContext *>(ctx->declarator())->Identifier()->getText();
  os << "\t.global " << name << "\n";
  os << name << ":" << "\n";

  for (auto stmt : ctx->statement()) {
    visit(stmt);
  }

  return {};
}

any Codegen::visitReturnStatement(CParser::ReturnStatementContext *ctx) {
  if (ctx->expression()) {
    visit(ctx->expression());
  }
  os << "\tret\n";

  return {};
}

any Codegen::visitIntegerConstantExpression(CParser::IntegerConstantExpressionContext *ctx) {
  load_imm("hl", stoi(ctx->getText()));

  return ExpressionCtx{};
}
