#include <argparse/argparse.hpp>
#include <cparse/lexer.hpp>
#include <cparse/parser.hpp>
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

  const auto path = args->get<std::filesystem::path>("input");
  auto intermediate = path;
  system((std::format("cpp -P {} -o {}", path.string(),
                      intermediate.replace_extension(".i").string()))
             .c_str());

  std::ifstream input(intermediate);
  cparse::Lexer lexer(input);
  cparse::Parser parser(lexer);
  auto tu = parser.translation_unit();

  return 0;
}
