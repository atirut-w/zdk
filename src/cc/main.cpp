#include <argparse/argparse.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <lexer.hpp>

using namespace std;
using namespace argparse;

unique_ptr<const ArgumentParser> parse_args(int argc, char *argv[])
{
    auto parser = make_unique<ArgumentParser>("cc");
    parser->add_description("C compiler for Z80");

    // Source file
    parser->add_argument("source").help("Source file").action([](const string &value) {
        return filesystem::absolute(value);
    });

    // Include directories
    parser->add_argument("-I", "--include")
        .help("Add directory to include search path")
        .action([](const string &value) { return filesystem::absolute(value); })
        .nargs(nargs_pattern::any)
        .default_value(vector<filesystem::path>{});

    // Keep intermediate files
    parser->add_argument("-k", "--keep-intermediate")
        .help("Keep intermediate files")
        .default_value(false)
        .implicit_value(true);

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

int main(int argc, char *argv[])
{
    auto args = parse_args(argc, argv);
    const auto source = args->get<filesystem::path>("source");
    filesystem::path intermediate = source;
    auto keep_intermediate = args->get<bool>("--keep-intermediate");

    string cpp_preamble = "clang -E -P -nostdinc -nostdinc++ ";
    for (const auto &include : args->get<vector<filesystem::path>>("--include"))
    {
        cpp_preamble += " -I" + include.string();
    }

    if (system((cpp_preamble + source.string() + " > " + intermediate.replace_extension(".i").string()).c_str()) != 0)
    {
        cerr << "Failed to preprocess source file" << endl;
        return 1;
    }

    ifstream input(intermediate);
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    return 0;
}
