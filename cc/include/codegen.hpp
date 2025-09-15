#pragma once
#include <cparse/ast.hpp>
#include <ostream>

class CodeGen {
  std::ostream &out;

public:
  CodeGen(std::ostream &out) : out(out) {}

  void visit(cparse::TranslationUnit &tu);
  void visit(cparse::FunctionDefinition &func);
  void visit(cparse::Statement &stmt);
  void visit(cparse::ReturnStatement &ret);

  void visit(cparse::Expression &expr, bool rhs = false);
  void visit(cparse::ConstantExpression &const_expr, bool rhs = false);
};
