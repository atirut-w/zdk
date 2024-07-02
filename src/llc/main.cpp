#include "ANTLRInputStream.h"
#include "tree/ParseTree.h"
#include <analyzer.hpp>
#include <argparse/argparse.hpp>
#include <compiler.hpp>
#include <filesystem>
#include <fstream>
#include <memory>

#include <LLVMIRLexer.h>
#include <LLVMIRParser.h>

using namespace std;
using namespace argparse;
using namespace antlr4;

unique_ptr<const ArgumentParser> parse_args(int argc, char *argv[])
{
    auto parser = make_unique<ArgumentParser>("llc");
    parser->add_description("LLVM IR compiler for Z80");

    // Source file
    parser->add_argument("source").help("LLVM IR source file").action([](const string &value) {
        return filesystem::absolute(value);
    });

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

    auto path = args->get<filesystem::path>("source");
    ifstream input(path);
    if (!input.is_open())
    {
        cerr << "Failed to open file: " << path << endl;
        return 1;
    }

    ANTLRInputStream stream(input);
    LLVMIRLexer lexer(&stream);
    CommonTokenStream tokens(&lexer);
    LLVMIRParser parser(&tokens);

    tree::ParseTree *tree = parser.compilationUnit();
    if (lexer.getNumberOfSyntaxErrors() > 0 || parser.getNumberOfSyntaxErrors() > 0)
    {
        return 1;
    }

    Analyzer analyzer;
    auto module_info = any_cast<ModuleInfo>(analyzer.visit(tree));

    ofstream output(path.replace_extension(".s"));
    Compiler compiler(module_info, output);
    compiler.visit(tree);

    return 0;
}
