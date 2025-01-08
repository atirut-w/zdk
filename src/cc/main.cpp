#include "lexer.hpp"
#include <argparse/argparse.hpp>
#include <filesystem>
#include <fstream>
#include <memory>

using namespace std;
using namespace argparse;

unique_ptr<const ArgumentParser> parse_args(int argc, char *argv[]) {
  auto parser = make_unique<ArgumentParser>();
  parser->add_description("C compiler for the Z80");

  // Source file
  parser->add_argument("source")
      .help("Source file")
      .action([](const string &value) { return filesystem::path(value); });

  try {
    parser->parse_args(argc, argv);
  } catch (const exception &e) {
    cerr << e.what() << endl;
    cerr << *parser << endl;
    return nullptr;
  }
  return parser;
}

int main(int argc, char *argv[]) {
  auto parser = parse_args(argc, argv);
  if (!parser) {
    return 1;
  }
  auto path = parser->get<filesystem::path>("source");

  ifstream file(path);
  if (!file) {
    cerr << "error: could not open file " << path << endl;
    return 1;
  }

  Lexer lexer(file);
  while (auto token = lexer.next()) {
    cout << "Token type " << token->type;
    if (token->type == Token::INTLIT) {
      cout << " value " << token->value;
    }
    cout << "\n";
  }

  return 0;
}
