#include <argparse/argparse.hpp>
#include <memory>
#include <filesystem>

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
    auto parser = parse_args(argc, argv);
    auto path = parser->get<filesystem::path>("source");

    return 0;
}
