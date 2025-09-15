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

struct ScopeMark {
  size_t sym_top;     // symbols.size() at entry
  int saved_off;      // frame_local_off at entry
  int bytes_in_block; // total bytes allocated for this block
};

class CodeGen {
  std::ostream &out;
  int next_label = 0;
  // NOTE: Includes ALL symbols, local and global
  std::vector<std::unique_ptr<Symbol>> symbols;
  cparse::FunctionDefinition *current_function = nullptr;

  // Single-pass local allocation state
  int frame_local_off = 0; // next negative offset from IX (0, -2, -4, ...)
  std::vector<ScopeMark> scope_stack;

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
  std::string format_ix(int offset) {
    if (offset == 0) {
      return "(ix)";
    } else if (offset > 0) {
      return std::format("(ix+{})", offset);
    } else {
      return std::format("(ix-{})", -offset);
    }
  }

  // Single-pass local allocation helpers
  void adjust_sp(int bytes);
  void enter_scope();
  void leave_scope();
  int allocate_block_locals(
      const std::vector<std::unique_ptr<cparse::Declaration>> &decls);
  int size_of_int() { return 2; } // placeholder; expand when adding types

public:
  CodeGen(std::ostream &out) : out(out) {}

  void visit(cparse::TranslationUnit &tu);
  void visit(cparse::FunctionDefinition &func);
  void visit(cparse::Block &block);
  void visit(cparse::Statement &stmt);
  void visit(cparse::ReturnStatement &ret);
  void visit(cparse::IfStatement &if_stmt);
  void visit(cparse::CompoundStatement &compound);

  void visit(cparse::Expression &expr, bool rhs = false);
  void visit(cparse::ConstantExpression &const_expr, bool rhs = false);
  void visit(cparse::UnaryExpression &unary_expr, bool rhs = false);
  void visit(cparse::BinaryExpression &bin_expr, bool rhs = false);
  void visit(cparse::AssignmentExpression &assign_expr, bool rhs = false);
  void visit(cparse::IdentifierExpression &id_expr, bool rhs = false);
  void visit(cparse::ConditionalExpression &cond_expr, bool rhs = false);
};
