#include <argparse/argparse.hpp>
#include <cparse/lexer.hpp>
#include <cparse/parser.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <ir_emitter.hpp>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <memory>

using argparse::ArgumentParser;

std::unique_ptr<ArgumentParser> parseArguments(int argc, char **argv) {
  auto parser = std::make_unique<ArgumentParser>("zdk-cc");

  parser->add_argument("input")
      .help("Input file to compile")
      .action(
          [](const std::string &arg) { return std::filesystem::path(arg); });

  parser->add_argument("--emit-llvm").help("Emit LLVM IR").flag();

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

  system((std::format("rm {}", intermediate.string())).c_str());

  llvm::LLVMContext context;
  auto module = std::make_unique<llvm::Module>(path.string(), context);
  IREmitter emitter(context, *module);
  emitter.visit(*tu);

  if (args->get<bool>("--emit-llvm")) {
    std::error_code ec;
    llvm::raw_fd_ostream ir_file(intermediate.replace_extension(".ll").string(),
                                 ec);
    if (ec) {
      std::cerr << "Error opening file for IR output: " << ec.message()
                << std::endl;
      return 1;
    }
    module->print(ir_file, nullptr);
    return 0;
  }

  return 0;
}
