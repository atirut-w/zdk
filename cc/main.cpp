#include "ANTLRInputStream.h"
#include "ast.hpp"
#include "ast_builder.hpp"
// #include "codegen.hpp"
#include "error.hpp"
#include <CLexer.h>
#include <CParser.h>
#include <argparse/argparse.hpp>
#include <cstdlib>
#include <error.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
// #include "analyzer.hpp"

using namespace std;
using namespace argparse;
using namespace antlr4;
// using namespace ZIR;

unique_ptr<const ArgumentParser> parse_args(int argc, char *argv[]) {
  auto parser = make_unique<ArgumentParser>("cc");
  parser->add_description("C compiler for Z80");

  // Source file
  parser->add_argument("source")
      .help("Source file")
      .action([](const string &value) { return filesystem::path(value); });

  // Include directories
  parser->add_argument("-I", "--include")
      .help("Add directory to include search path")
      .action([](const string &value) { return filesystem::path(value); })
      .append();

  ArgumentParser::MutuallyExclusiveGroup &stage =
      parser->add_mutually_exclusive_group(false);

  // Preprocess only
  stage.add_argument("-E")
      .help("Preprocess the input file, but do not compile")
      .flag();

  // Codegen only
  stage.add_argument("-S")
      .help("Compile the input file, but do not assemble or link")
      .flag();

  // Assemble only
  stage.add_argument("-c")
      .help("Compile and assemble the input file, but do not link")
      .flag();

  // Dump AST
  parser->add_argument("--dump-tree").help("Dump parse tree to stdout").flag();

  // // Emit LLVM IR
  // parser->add_argument("--emit-llvm").help("Emit LLVM IR").flag();

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

// bool validate(const string &preamble, const filesystem::path &source)
// {
// #ifdef CLANG_VALIDATION
//     return system(("clang " + preamble + "-fsyntax-only " +
//     source.string()).c_str()) == 0;
// #else
//     return true;
// #endif
// }

bool preprocess(const vector<string> &preamble, const filesystem::path &source,
                const filesystem::path &intermediate) {
  vector<string> args = preamble;
  args.push_back("-P"); // No line markers please
  args.push_back(source);
  args.push_back("-o");
  args.push_back(intermediate);

  if (run("cpp", args) != 0) {
    filesystem::remove(intermediate);
    return false;
  }
  return true;
}

bool assemble(const filesystem::path source,
              const filesystem::path intermediate) {
  return run("z80-elf-as", {source, "-o", intermediate}) == 0;
}

bool link(const filesystem::path source, const filesystem::path intermediate) {
  return run("z80-elf-ld", {source, "-o", intermediate}) == 0;
}

int main(int argc, char *argv[]) {
  auto args = parse_args(argc, argv);
  const auto source = args->get<filesystem::path>("source");
  filesystem::path intermediate = source;

  vector<string> cc_preamble = {"-nostdinc", "-nostdlib"};
  auto includes = args->get<vector<filesystem::path>>("--include");
  for (auto &include : includes) {
    cc_preamble.push_back("-I" + include.string());
  }

  // if (!validate(cc_preamble, source))
  // {
  //     return 1;
  // }
  if (!preprocess(cc_preamble, source, intermediate.replace_extension(".i"))) {
    return 1;
  }
  if (args->get<bool>("-E")) {
    return 0;
  }

  ifstream input(intermediate.replace_extension(".i"));
  ANTLRInputStream input_stream(input);
  input_stream.name = source;

  CLexer lexer(&input_stream);
  CommonTokenStream tokens(&lexer);
  CParser parser(&tokens);

  lexer.removeErrorListeners();
  parser.removeErrorListeners();
  auto listener = make_unique<CCErrorListener>();
  lexer.addErrorListener(listener.get());
  parser.addErrorListener(listener.get());

  tokens.fill();
  tree::ParseTree *tree = parser.translationUnit();
  input.close();
  filesystem::remove(intermediate.replace_extension(".i"));

  if (lexer.getNumberOfSyntaxErrors() > 0 ||
      parser.getNumberOfSyntaxErrors() > 0) {
    return {};
  } else if (args->get<bool>("--dump-tree")) {
    cout << tree->toStringTree(&parser, true) << endl;
    return {};
  }

  // ASTBuilder builder;
  // auto ast = unique_ptr<Expression>(any_cast<Expression *>(builder.visit(tree)));

  // Analyzer analyzer;
  // analyzer.visit(*ast);

  // ofstream output(intermediate.replace_extension(".s"));
  // Codegen codegen(output);
  // codegen.visit(*ast);

  if (args->get<bool>("-S")) {
    return 0;
  }
  if (!assemble(intermediate.replace_extension(".s"),
                intermediate.replace_extension(".o"))) {
    return 1;
  }
  filesystem::remove(intermediate.replace_extension(".s"));

  if (args->get<bool>("-c")) {
    return 0;
  }
  if (!link(intermediate.replace_extension(".o"),
            intermediate.replace_extension(".elf"))) {
    return 1;
  }
  filesystem::remove(intermediate.replace_extension(".o"));

  return 0;
}
