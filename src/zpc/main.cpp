#include "ANTLRInputStream.h"
#include "argparse/argparse.hpp"
#include "pascalLexer.h"
#include "pascalParser.h"
#include "tree/ParseTree.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

using namespace std;
using namespace argparse;
using namespace antlr4;

unique_ptr<const ArgumentParser> parse_args(int argc, char *argv[]) {
  auto parser = make_unique<ArgumentParser>();

  parser->add_argument("source")
      .help("Source file")
      .action([](const string &value) { return filesystem::path(value); });

  try {
    parser->parse_args(argc, argv);
    return parser;
  } catch (const runtime_error &e) {
    cerr << e.what() << endl;
    cerr << *parser << endl;
    return nullptr;
  }
}

int main(int argc, char *argv[]) {
  auto args = parse_args(argc, argv);
  const auto &source = args->get<filesystem::path>("source");

  ifstream input(source);
  if (!input) {
    cerr << "Error: could not open source file" << endl;
    return 1;
  }

  ANTLRInputStream antlr_input(input);
  antlr_input.name = source;
  pascalLexer lexer(&antlr_input);
  CommonTokenStream tokens(&lexer);
  pascalParser parser(&tokens);

  tree::ParseTree *tree = parser.program();
  if (lexer.getNumberOfSyntaxErrors() > 0 || parser.getNumberOfSyntaxErrors() > 0) {
    return 1;
  }

  return 0;
}
