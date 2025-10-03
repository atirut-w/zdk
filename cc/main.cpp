#include "analyzer.hpp"
#include "codegen.hpp"
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

  auto path = args->get<std::filesystem::path>("input");
  auto preprocessed = path.replace_extension(".i");
  auto assembly = path.replace_extension(".s");
  auto object = path.replace_extension(".o");
  auto binary = path.replace_extension("");

  system((std::format("cpp -P {} -o {}", path.replace_extension(".c").string(),
                      preprocessed.string()))
             .c_str());

  std::ifstream input(preprocessed);
  cparse::Lexer lexer(input);
  cparse::Parser parser(lexer);
  auto tu = parser.translation_unit();
  system((std::format("rm {}", preprocessed.string())).c_str());

  Analyzer analyzer;
  analyzer.visit(*tu);

  std::ofstream output(assembly);
  CodeGen codegen(output, analyzer);
  codegen.visit(*tu);

  if (args->get<bool>("-S")) {
    return 0;
  }

  if (system((std::format("z80-elf-as {} -o {}", assembly.string(),
                          object.string()))
                 .c_str())) {
    return 1;
  }
  system((std::format("rm {}", assembly.string())).c_str());

  if (system(
          (std::format("z80-elf-ld -o {} {}", binary.string(), object.string()))
              .c_str())) {
    return 1;
  }
  system((std::format("rm {}", object.string())).c_str());

  return 0;
}
