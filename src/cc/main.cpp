#include <argparse/argparse.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

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
        .nargs(nargs_pattern::any);

    ArgumentParser::MutuallyExclusiveGroup &stage = parser->add_mutually_exclusive_group(false);

    // Preprocess only
    stage.add_argument("-E")
        .help("Preprocess the input file, but do not compile")
        .default_value(false)
        .implicit_value(true);

    // Codegen only
    stage.add_argument("-S")
        .help("Compile the input file, but do not assemble or link")
        .default_value(false)
        .implicit_value(true);

    // Assemble only
    stage.add_argument("-c")
        .help("Compile and assemble the input file, but do not link")
        .default_value(false)
        .implicit_value(true);

    // Dump AST
    parser->add_argument("--dump-ast").help("Dump AST to stdout").default_value(false).implicit_value(true);

    try
    {
        parser->parse_args(argc, argv);
    }
    catch (const exception &e)
    {
        cerr << e.what() << endl;
        cerr << *parser << endl;
        exit(1);
    }
    return parser;
}

int main(int argc, char *argv[])
{
    auto args = parse_args(argc, argv);
    const auto source = args->get<filesystem::path>("source");
    filesystem::path intermediate = source;

    string cpp_preamble = "exec -a zdk-cpp clang -E -P -nostdinc -nostdinc++ ";
    for (const auto &include : args->get<vector<filesystem::path>>("--include"))
    {
        cpp_preamble += " -I" + include.string();
    }

    if (system((cpp_preamble + source.string() + " > " + intermediate.replace_extension(".i").string()).c_str()) != 0)
    {
        cerr << "Failed to preprocess source file" << endl;
        return 1;
    }
    if (args->get<bool>("-E"))
    {
        return 0;
    }

    if (args->get<bool>("-S"))
    {
        return 0;
    }
    if (system(("z80-elf-as " + intermediate.replace_extension(".s").string() + " -o " +
                intermediate.replace_extension(".o").string())
                   .c_str()))
    {
        return 1;
    }
    filesystem::remove(intermediate.replace_extension(".s"));

    if (args->get<bool>("-c"))
    {
        return 0;
    }
    if (system(("z80-elf-ld " + intermediate.replace_extension(".o").string() + " -o a.out").c_str()))
    {
        return 1;
    }
    filesystem::remove(intermediate.replace_extension(".o"));

    return 0;
}
