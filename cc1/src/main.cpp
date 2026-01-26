#include "lexer.hpp"
#include <format>
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: cc1 <source-file>\n";
    return 1;
  }

  std::ifstream source_file(argv[1]);
  if (!source_file.is_open()) {
    std::cerr << "Error: Could not open file " << argv[1] << "\n";
    return 1;
  }

  Lexer lexer(source_file);
  while (auto token = lexer.next_token()) {
    std::cout << std::format("Token {} @ {}:{}\n", static_cast<int>(token->kind), token->line, token->column);
  }
}
