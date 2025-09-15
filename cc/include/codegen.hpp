#pragma once
#include <cparse/ast.hpp>
#include <ostream>

struct Symbol {
  std::string name;
  virtual ~Symbol() = default;
};

struct LocalVariable : public Symbol {
  int offset; // Offset from the frame pointer
};

class CodeGen {
  std::ostream &out;
  int next_label = 0;
  // NOTE: Includes ALL symbols, local and global
  std::vector<std::unique_ptr<Symbol>> symbols;
  cparse::FunctionDefinition *current_function = nullptr;

  int generate_label() { return next_label++; }
  Symbol *find_symbol(const std::string &name) {
    for (auto it = symbols.end(); it != symbols.begin();) {
      --it;
      if ((*it)->name == name) {
        return it->get();
      }
    }
    return nullptr;
  }

public:
  CodeGen(std::ostream &out) : out(out) {}

  void visit(cparse::TranslationUnit &tu);
  void visit(cparse::FunctionDefinition &func);
  void visit(cparse::Statement &stmt);
  void visit(cparse::ReturnStatement &ret);

  void visit(cparse::Expression &expr, bool rhs = false);
  void visit(cparse::ConstantExpression &const_expr, bool rhs = false);
  void visit(cparse::UnaryExpression &unary_expr, bool rhs = false);
  void visit(cparse::BinaryExpression &bin_expr, bool rhs = false);
  void visit(cparse::AssignmentExpression &assign_expr, bool rhs = false);
  void visit(cparse::IdentifierExpression &id_expr, bool rhs = false);
};
