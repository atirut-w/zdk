#include "argparse/argparse.hpp"
#include "codegen.hpp"
#include "legalizer.hpp"
#include "liveness_analyzer.hpp"
#include <filesystem>
#include <fstream>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/IRPrinter/IRPrintingPasses.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>
#include <system_error>

using namespace std;
using namespace argparse;
using namespace llvm;

unique_ptr<const ArgumentParser> parse_args(int argc, char **argv) {
  auto parser = make_unique<ArgumentParser>("llc");

  parser->add_argument("input")
      .help("Input file")
      .action([](const string &value) { return filesystem::path(value); });

  try {
    parser->parse_args(argc, argv);
    return parser;
  } catch (const runtime_error &e) {
    cerr << e.what() << endl;
    cerr << *parser << endl;
  }
  return nullptr;
}

int main(int argc, char **argv) {
  auto args = parse_args(argc, argv);
  if (!args) {
    return 1;
  }

  auto input = args->get<filesystem::path>("input");
  LLVMContext context;
  SMDiagnostic error;
  auto module = parseIRFile(input.string(), error, context);
  if (!module) {
    error.print("zdk-llc", errs());
    return 1;
  }

  Legalizer legalizer;
  legalizer.visit_module(*module);
  error_code ec;
  raw_fd_ostream dump(input.replace_extension("legalized.ll").string(), ec);
  module->print(dump, nullptr);
  return 0;

  ofstream os(input.replace_extension(".s"));
  Codegen codegen(os);
  codegen.visit_module(*module);

  return 0;
}
