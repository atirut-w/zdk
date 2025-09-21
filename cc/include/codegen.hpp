#pragma once
#include <cparse/ast.hpp>
#include <cparse/visitor.hpp>
#include <ostream>

struct Symbol {
  std::string name;
  virtual ~Symbol() = default;
};

struct LocalVariable : public Symbol {
  int offset; // Offset from the frame pointer
};

struct ScopeMark {
  size_t sym_top;     // symbols.size() at entry
  int saved_off;      // frame_local_off at entry
  int bytes_in_block; // total bytes allocated for this block
};

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
