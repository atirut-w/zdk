#include <argparse/argparse.hpp>
#include <filesystem>
#include <format>
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
  auto parser = parseArguments(argc, argv);
  if (!parser) {
    return 1;
  }

  auto input = parser->get<std::filesystem::path>("input");
  system((std::format("cpp -P {} -o {}", input.string(),
                      input.replace_extension(".i").string()))
             .c_str());

  return 0;
}
