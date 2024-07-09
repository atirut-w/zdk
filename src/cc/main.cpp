#include <CLexer.h>
#include <CParser.h>
#include <analyzer.hpp>
#include <argparse/argparse.hpp>
#include <codegen.hpp>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

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

    // Keep intermediate files
    parser->add_argument("-k", "--keep-intermediate")
        .help("Keep intermediate files")
        .default_value(false)
        .implicit_value(true);
    
    // Dump AST
    parser->add_argument("--dump-ast")
        .help("Dump AST to stdout")
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

    string clang_preamble = "exec -a zdk-cc clang -nostdinc -nostdlib ";
    for (auto &include : args->get<vector<filesystem::path>>("--include"))
    {
        clang_preamble += "-I" + include.string() + " ";
    }

    if (system((clang_preamble + "-fsyntax-only " + source.string()).c_str()))
    {
        return 1;
    }
    if (system(
            (clang_preamble + "-E " + source.string() + " > " + intermediate.replace_extension(".i").string()).c_str()))
    {
        cerr << "BUG: clang -E failed despite syntax check passing" << endl;
        return 1;
    }

    ifstream input(intermediate.replace_extension(".i"));
    ANTLRInputStream input_stream(input);
    CLexer lexer(&input_stream);
    CommonTokenStream tokens(&lexer);
    CParser parser(&tokens);

    tree::ParseTree *tree = parser.compilationUnit();
    if (lexer.getNumberOfSyntaxErrors() > 0 || parser.getNumberOfSyntaxErrors() > 0)
    {
        cerr << "BUG: ANTLR syntax error. mayhapes C version newer than C11?" << endl;
        return 1;
    }

    if (!keep_intermediate)
    {
        filesystem::remove(intermediate.replace_extension(".i"));
    }
    std::ofstream output(intermediate.replace_extension(".s"));

    if (args->get<bool>("--dump-ast"))
    {
        cout << tree->toStringTree(&parser) << endl;
        return 0;
    }

    Analyzer analyzer;
    ProgramMeta meta = any_cast<ProgramMeta>(analyzer.visit(tree));
    CodeGen codegen(meta, output);
    codegen.visit(tree);

    return 0;
}
