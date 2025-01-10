#pragma once
#include <any>
#include <ostream>
#include <string>

class TranslationUnit;
class FunctionDefinition;

class ReturnStatement;

class Expression;
class IntegerConstant;
class BinaryExpression;

class Codegen {
  std::ostream &os;

  void load_imm(int value);

public:
  Codegen(std::ostream &os) : os(os) {}

  void visit(const TranslationUnit &node);
  void visit(const FunctionDefinition &node);

  void visit(const ReturnStatement &node);

  void visit(const Expression &node);
  void visit(const IntegerConstant &node);
  void visit(const BinaryExpression &node);
};
