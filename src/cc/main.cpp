#include <argparse/argparse.hpp>
#include <memory>
#include <filesystem>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;
using namespace argparse;

unique_ptr<const ArgumentParser> parse_args(int argc, char *argv[])
{
    auto parser = make_unique<ArgumentParser>("cc");
    parser->add_description("C compiler for Z80");

    // Source file
    parser->add_argument("source")
        .help("Source file")
        .action([](const string &value) { return filesystem::absolute(value); });
    
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
    
    string clang_preamble = "exec -a zdk-cc clang -nostdinc -nostdlib ";
    for (auto &include : args->get<vector<filesystem::path>>("--include"))
    {
        clang_preamble += "-I" + include.string() + " ";
    }

    if (system((clang_preamble + "-S -emit-llvm " + args->get<filesystem::path>("source").string()).c_str()))
    {
        return 1;
    }

    return 0;
}
