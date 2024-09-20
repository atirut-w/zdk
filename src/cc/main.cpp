#include "ANTLRInputStream.h"
#include "codegen.hpp"
#include "error.hpp"
#include "irgen.hpp"
#include <CLexer.h>
#include <CParser.h>
#include <argparse/argparse.hpp>
#include <config.hpp>
#include <cstdlib>
#include <error.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>
#include <zir/module.hpp>

using namespace std;
using namespace argparse;
using namespace antlr4;
using namespace ZIR;

unique_ptr<const ArgumentParser> parse_args(int argc, char *argv[])
{
    auto parser = make_unique<ArgumentParser>("cc");
    parser->add_description("C compiler for Z80");

    // Source file
    parser->add_argument("source").help("Source file").action([](const string &value) {
        return filesystem::path(value);
    });

    // Include directories
    parser->add_argument("-I", "--include")
        .help("Add directory to include search path")
        .action([](const string &value) { return filesystem::path(value); })
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

bool validate(const string &preamble, const filesystem::path &source)
{
#ifdef CLANG_VALIDATION
    return system(("clang " + preamble + "-fsyntax-only " + source.string()).c_str()) == 0;
#else
    return true;
#endif
}

bool preprocess(const string &preamble, const filesystem::path source, const filesystem::path intermediate)
{
    if (system(("cpp -P " + preamble + source.string() + " > " + intermediate.string()).c_str()) != 0)
    {
        filesystem::remove(intermediate);
        return false;
    }
    return true;
}

optional<const Module> compile_ir(const filesystem::path &source, bool dump_ast)
{
    ifstream input(source);
    ANTLRInputStream input_stream(input);
    input_stream.name = source.string();

    CLexer lexer(&input_stream);
    CommonTokenStream tokens(&lexer);
    CParser parser(&tokens);

    lexer.removeErrorListeners();
    parser.removeErrorListeners();
    auto listener = make_unique<CCErrorListener>();
    lexer.addErrorListener(listener.get());
    parser.addErrorListener(listener.get());

    tree::ParseTree *tree = parser.compilationUnit();
    if (lexer.getNumberOfSyntaxErrors() > 0 || parser.getNumberOfSyntaxErrors() > 0)
    {
        return {};
    }
    else if (dump_ast)
    {
        cout << tree->toStringTree(&parser, true) << endl;
        return {};
    }

    Module module;
    IRGen irgen(module);
    irgen.visit(tree);
    return module;
}

bool assemble(const filesystem::path source, const filesystem::path intermediate)
{
    return system(("z80-elf-as " + source.string() + " -o " + intermediate.string()).c_str()) == 0;
}

bool link(const filesystem::path source, const filesystem::path intermediate)
{
    return system(("z80-elf-ld " + source.string() + " -o " + intermediate.string()).c_str()) == 0;
}

int main(int argc, char *argv[])
{
    auto args = parse_args(argc, argv);
    const auto source = args->get<filesystem::path>("source");
    filesystem::path intermediate = source;

    string cc_preamble = "-nostdinc -nostdlib ";
    auto includes = args->get<vector<filesystem::path>>("--include");
    for (auto &include : includes)
    {
        cc_preamble += "-I" + include.string() + " ";
    }

    if (!validate(cc_preamble, source))
    {
        return 1;
    }
    if (!preprocess(cc_preamble, source, intermediate.replace_extension(".i")))
    {
        return 1;
    }
    if (args->get<bool>("-E"))
    {
        return 0;
    }

    auto module = compile_ir(intermediate.replace_extension(".i"), args->get<bool>("--dump-ast"));
    filesystem::remove(intermediate.replace_extension(".i"));
    if (!module)
    {
        if (args->get<bool>("--dump-ast"))
        {
            return 0;
        }
        return 1;
    }

    std::ofstream output(intermediate.replace_extension(".s"));
    Codegen codegen(output, *module);
    codegen.generate();

    if (args->get<bool>("-S"))
    {
        return 0;
    }
    if (!assemble(intermediate.replace_extension(".s"), intermediate.replace_extension(".o")))
    {
        return 1;
    }
    filesystem::remove(intermediate.replace_extension(".s"));

    if (args->get<bool>("-c"))
    {
        return 0;
    }
    if (!link(intermediate.replace_extension(".o"), intermediate.replace_extension(".elf")))
    {
        return 1;
    }
    filesystem::remove(intermediate.replace_extension(".o"));

    return 0;
}
