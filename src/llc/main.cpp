#include "argparse/argparse.hpp"
#include <filesystem>
#include <memory>

using namespace std;
using namespace argparse;

unique_ptr<const ArgumentParser> parse_args(int argc, char **argv) {
  auto parser = make_unique<ArgumentParser>("llc");

  parser->add_argument("input")
      .help("Input file")
      .action([](const string &value) { return filesystem::path(value); });

  try {
    parser->parse_args(argc, argv);
    return parser;
  } catch (const runtime_error &e) {
    cerr << e.what() << endl;
    cerr << *parser << endl;
  }
  return nullptr;
}

int main(int argc, char **argv) {
  auto args = parse_args(argc, argv);
  if (!args) {
    return 1;
  }

  return 0;
}
