#include "backend/asm_printer.hpp"
#include <argparse/argparse.hpp>
#include <filesystem>
#include <fstream>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <memory>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;
using namespace argparse;
using namespace llvm;

unique_ptr<const ArgumentParser> parse_args(int argc, char *argv[]) {
  auto parser = make_unique<ArgumentParser>("llc");
  parser->add_description("ZDK LLVM Compiler");

  // Source file
  parser->add_argument("source").help("Source file").action([](const string &value) { return filesystem::path(value); });

  // Output file
  parser->add_argument("-o", "--output").help("Output file").action([](const string &value) { return filesystem::path(value); });

  try {
    parser->parse_args(argc, argv);
  } catch (const exception &e) {
    cerr << e.what() << endl;
    cerr << *parser << endl;
    exit(1);
  }
  return parser;
}

int run(string program, vector<string> args) {
  pid_t pid = fork();

  if (pid < 0) {
    throw runtime_error("failed to fork");
  } else if (pid == 0) {
    vector<char *> c_args;
    c_args.push_back(const_cast<char *>(program.c_str()));
    for (auto &arg : args) {
      c_args.push_back(const_cast<char *>(arg.c_str()));
    }
    c_args.push_back(nullptr);

    execvp(program.c_str(), c_args.data());
    _exit(EXIT_FAILURE);
  } else {
    int status;
    waitpid(pid, &status, 0);
    return WEXITSTATUS(status);
  }
}

int main(int argc, char *argv[]) {
  auto args = parse_args(argc, argv);

  const auto source = args->get<filesystem::path>("source");
  LLVMContext context;
  SMDiagnostic err;
  auto module = parseIRFile(source.string(), err, context);

  if (!module) {
    err.print("zdk-llc", llvm::errs());
    return 1;
  }


  const auto output = args->is_used("--output") ? args->get<filesystem::path>("--output") : filesystem::path(source).replace_extension(".s");
  ofstream output_stream(output);
  AsmPrinter(*module, output_stream).print();

  return 0;
}
