#pragma once
#include <cparse/ast.hpp>
#include <cparse/visitor.hpp>
#include <ostream>

class CodeGen : public cparse::Visitor<void> {
  std::ostream &out;

public:
  CodeGen(std::ostream &out) : out(out) {}

  using cparse::Visitor<void>::visit;

  void visit(cparse::TranslationUnit &node) override;
  void visit(cparse::FunctionDefinition &node) override;
  void visit(cparse::ReturnStatement &node) override;

  void visit(cparse::ConstantExpression &node) override;
};
