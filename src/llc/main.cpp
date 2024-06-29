#include <argparse/argparse.hpp>
#include <memory>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <filesystem>
#include <iostream>
#include <llvm/Support/SourceMgr.h>

using namespace std;
using namespace argparse;
using namespace llvm;

unique_ptr<const ArgumentParser> parse_args(int argc, char *argv[])
{
    auto parser = make_unique<ArgumentParser>("llc");
    parser->add_description("LLVM IR compiler for Z80");

    // Source file
    parser->add_argument("source")
        .help("LLVM IR file")
        .action([](const string &value) { return filesystem::absolute(value); });

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

    LLVMContext context;
    SMDiagnostic error;
    auto module = parseIRFile(path.string(), error, context);
    if (!module)
    {
        error.print("llc", errs());
        return 1;
    }
    
    return 0;
}
