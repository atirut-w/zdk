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

bool validate_input(const filesystem::path &path, vector<filesystem::path> includes)
{
    if (!filesystem::exists(path))
    {
        cerr << "File not found: " << path << endl;
        return false;
    }

    string command = "clang -fsyntax-only -nostdinc -nostdlib ";
    for (const auto &include : includes)
    {
        command += "-I" + include.string() + " ";
    }

    return system((command + path.string()).c_str()) == 0;
}

int main(int argc, char *argv[])
{
    auto args = parse_args(argc, argv);
    auto path = filesystem::absolute(args->get<filesystem::path>("source"));
    auto includes = args->get<vector<filesystem::path>>("include");

    if (!validate_input(path, includes))
    {
        return 1;
    }

    return 0;
}
