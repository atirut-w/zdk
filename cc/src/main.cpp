#include <argparse/argparse.hpp>
#include <memory>
#include <filesystem>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <clang/Tooling/CommonOptionsParser.h>
#include <llvm/Support/CommandLine.h>

using namespace std;
using namespace argparse;
using namespace clang::tooling;
using namespace llvm;

static cl::OptionCategory ccCategory("cc options");

unique_ptr<const ArgumentParser> parse_args(int argc, char *argv[])
{
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
        .nargs(1)
        .default_value(vector<filesystem::path>{});

    try
    {
        parser->parse_args(argc, argv);
    }
    catch (const exception &e)
    {
        cerr << e.what() << endl;
        exit(1);
    }
    return parser;
}

int main(int argc, const char *argv[])
{
    auto expected_parser = CommonOptionsParser::create(argc, argv, ccCategory);

    return 0;
}
