#include <argparse/argparse.hpp>
#include <memory>

using namespace std;
using namespace argparse;

unique_ptr<const ArgumentParser> parse_args(int argc, char *argv[])
{
    auto parser = make_unique<ArgumentParser>("llc");
    parser->add_description("LLVM IR compiler for Z80");

    // Source file
    parser->add_argument("source")
        .help("LLVM IR file");

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
    
    return 0;
}
