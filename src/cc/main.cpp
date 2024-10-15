#include "argparse/argparse.hpp"
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

using namespace std;
using namespace argparse;

using ParsedArgs =
    tuple<unique_ptr<const ArgumentParser>, const vector<string>>;

optional<ParsedArgs> parse_args(int argc, char *argv[]) {
  auto parser = make_unique<ArgumentParser>();

  parser->add_argument("file")
      .help("C source file to compile")
      .action([](const string &value) { return filesystem::path(value); });

  try {
    auto remaining = parser->parse_known_args(argc, argv);
    return {{std::move(parser), std::move(remaining)}};
  } catch (const runtime_error &e) {
    cerr << e.what() << endl;
    cerr << *parser << endl;
    return nullopt;
  }
}

int main(int argc, char *argv[]) {
  auto expected_args = parse_args(argc, argv);
  if (!expected_args) {
    return 1;
  }
  auto &[parser, remaining] = *expected_args;
}
