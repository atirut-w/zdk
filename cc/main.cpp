#include <ANTLRFileStream.h>
#include <ANTLRInputStream.h>
#include <CLexer.h>
#include <CParser.h>
#include <CommonTokenStream.h>
#include <argparse/argparse.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>

using argparse::ArgumentParser;

std::unique_ptr<ArgumentParser> parseArguments(int argc, char **argv) {
  auto parser = std::make_unique<ArgumentParser>("zdk-cc");

  parser->add_argument("input")
      .help("Input file to compile")
      .action(
          [](const std::string &arg) { return std::filesystem::path(arg); });

  parser->add_argument("-S").help("Compile to assembly only").flag();

  try {
    parser->parse_args(argc, argv);
    return parser;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    std::cerr << *parser;
    return nullptr;
  }
}

int main(int argc, char **argv) {
  auto args = parseArguments(argc, argv);
  if (!args) {
    return 1;
  }

  auto input = args->get<std::filesystem::path>("input");
  if (system((std::format("cpp -P {} -o {}", input.string(),
                          input.replace_extension(".i").string()))
                 .c_str())) {
    return 1;
  }

  antlr4::ANTLRFileStream inputStream;
  inputStream.loadFromFile(input.replace_extension(".i").string());

  CLexer lexer(&inputStream);
  antlr4::CommonTokenStream tokens(&lexer);

  tokens.fill();

  CParser parser(&tokens);
  auto tree = parser.program();

  return 0;
}
