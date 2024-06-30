#include <argparse/argparse.hpp>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <vector>
#include <fstream>
#include <CLexer.h>
#include <CParser.h>

using namespace std;
using namespace argparse;
using namespace antlr4;

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
    
    const auto path = args->get<filesystem::path>("source");
    string clang_preamble = "exec -a zdk-cc clang -std=c90 ";
    for (auto &include : args->get<vector<filesystem::path>>("--include"))
    {
        clang_preamble += "-I" + include.string() + " ";
    }

    if (system((clang_preamble + "-fsyntax-only -nostdinc -nostdlib " + path.string()).c_str()) != 0)
    {
        return 1;
    }

    filesystem::path preprocessed_path = path;
    preprocessed_path.replace_extension(".i");
    system((clang_preamble + "-E " + path.string() + " > " + preprocessed_path.string()).c_str());
    ifstream stream(preprocessed_path.string());

    ANTLRInputStream input(stream);
    CLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    CParser parser(&tokens);

    tree::ParseTree *tree = parser.translationUnit();
    cout << tree->toStringTree(&parser) << endl;

    return 0;
}
