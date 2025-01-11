#include "codegen.hpp"
#include "ast.hpp"
#include <memory>

using namespace std;

enum Register {
  REG_A = 1 << 6,
  REG_B = 1 << 5,
  REG_C = 1 << 4,
  REG_D = 1 << 3,
  REG_E = 1 << 2,
  REG_H = 1 << 1,
  REG_L = 1 << 0,
  REG_BC = REG_B | REG_C,
  REG_DE = REG_D | REG_E,
  REG_HL = REG_H | REG_L,
};

int used_regs = 0;

map<int, string> reg_map = {
    {Register::REG_A, "a"}, {Register::REG_B, "b"}, {Register::REG_C, "c"},   {Register::REG_D, "d"},   {Register::REG_E, "e"},
    {Register::REG_H, "h"}, {Register::REG_L, "l"}, {Register::REG_BC, "bc"}, {Register::REG_DE, "de"}, {Register::REG_HL, "hl"},
};

vector<int> GPR16 = {Register::REG_BC, Register::REG_DE, Register::REG_HL};

void Codegen::gen_expression(Expression *expr) {
  if (auto *ic = dynamic_cast<IntegerConstant *>(expr)) {
    os << "\tld hl, " << ic->value << "\n";
  } else if (auto *be = dynamic_cast<BinaryExpression *>(expr)) {
    gen_expression(be->right.get());
    os << "\tpush hl\n";
    gen_expression(be->left.get());

    switch (be->op) {
    case BinaryExpression::Operator::Add:
      os << "\tpop de\n";
      os << "\tadd hl, de\n";
      break;
    case BinaryExpression::Operator::Sub:
      os << "\tpop de\n";
      os << "\tor a\n";
      os << "\tsbc hl, de\n";
      break;
    case BinaryExpression::Operator::Mul:
    case BinaryExpression::Operator::Div:
    case BinaryExpression::Operator::Mod:
      os << "\tpush hl\n";

      switch (be->op) {
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
}

any Codegen::visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) {
  string name = dynamic_cast<CParser::FunctionDeclaratorContext *>(ctx->declarator())->Identifier()->getText();
  os << name << ":" << endl;

  for (auto stmt : ctx->statement()) {
    visit(stmt);
  }

  return {};
}

any Codegen::visitReturnStatement(CParser::ReturnStatementContext *ctx) {
  if (ctx->expression()) {
    auto expr = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression())));
    gen_expression(expr.get());
  }
  os << "\tret" << "\n";

  return {};
}

any Codegen::visitIntegerConstantExpression(CParser::IntegerConstantExpressionContext *ctx) {
  auto ic = new IntegerConstant();
  ic->value = stoi(ctx->getText());

  return static_cast<Expression *>(ic);
}

any Codegen::visitParenthesizedExpression(CParser::ParenthesizedExpressionContext *ctx) { return visit(ctx->expression()); }

any Codegen::visitMultiplicativeExpression(CParser::MultiplicativeExpressionContext *ctx) {
  auto be = new BinaryExpression();

  be->left = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression(0))));
  be->right = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression(1))));

  if (ctx->Multiply()) {
    be->op = BinaryExpression::Operator::Mul;
  } else if (ctx->Divide()) {
    be->op = BinaryExpression::Operator::Div;
  } else if (ctx->Modulo()) {
    be->op = BinaryExpression::Operator::Mod;
  }

  return static_cast<Expression *>(be);
}

any Codegen::visitAdditiveExpression(CParser::AdditiveExpressionContext *ctx) {
  auto be = new BinaryExpression();

  be->left = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression(0))));
  be->right = unique_ptr<Expression>(any_cast<Expression *>(visit(ctx->expression(1))));

  if (ctx->Add()) {
    be->op = BinaryExpression::Operator::Add;
  } else if (ctx->Subtract()) {
    be->op = BinaryExpression::Operator::Sub;
  }

  return static_cast<Expression *>(be);
}
