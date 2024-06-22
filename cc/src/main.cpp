#include <argparse/argparse.hpp>
#include <memory>
#include <filesystem>
#include <iostream>
#include <vector>
#include <clang-c/Index.h>

using namespace std;
using namespace argparse;

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

int main(int argc, char *argv[])
{
    auto args = parse_args(argc, argv);
    auto path = filesystem::absolute(args->get<filesystem::path>("source"));
    auto includes = args->get<vector<filesystem::path>>("include");
    // includes.push_back("cc/include");

    vector<string> clang_args;
    clang_args.push_back("-fsyntax-only");
    clang_args.push_back("-nostdinc");
    clang_args.push_back("-nostdlib");
    for (const auto &include : includes)
    {
        clang_args.push_back("-I");
        clang_args.push_back(include.string());
    }

    const char *argv2[clang_args.size() + 1];
    for (size_t i = 0; i < clang_args.size(); i++)
    {
        argv2[i] = clang_args[i].c_str();
    }

    CXIndex index = clang_createIndex(0, 0);
    CXTranslationUnit tu = clang_parseTranslationUnit(index, path.c_str(), argv2, clang_args.size(), nullptr, 0, CXTranslationUnit_None);

    if (clang_getNumDiagnostics(tu) > 0)
    {
        for (unsigned i = 0; i < clang_getNumDiagnostics(tu); i++)
        {
            CXDiagnostic diag = clang_getDiagnostic(tu, i);
            CXSourceLocation loc = clang_getDiagnosticLocation(diag);
            CXString msg = clang_formatDiagnostic(diag, clang_defaultDiagnosticDisplayOptions());
            CXString file;
            unsigned line, column;
            clang_getPresumedLocation(loc, &file, &line, &column);
            cout << clang_getCString(msg) << endl;
            clang_disposeString(file);
            clang_disposeString(msg);
            clang_disposeDiagnostic(diag);
        }
    }

    return 0;
}
