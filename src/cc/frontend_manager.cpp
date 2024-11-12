#include "argparse/argparse.hpp"
#include "frotnend_manager.hpp"
#include <exception>
#include <iostream>

using namespace std;
using namespace argparse;

void FrontendManager::add_action(std::unique_ptr<FrontendAction> action) { actions.push_back(std::move(action)); }

bool FrontendManager::parse_args(int argc, char *argv[]) {
  parser.add_description("C compiler for the Z80");

  parser.add_argument("input").help("Input files").nargs(nargs_pattern::at_least_one).action([&](const string &value) {
    return filesystem::path(value);
  });

  try {
    for (auto &action : actions) {
      action->register_options(parser);
    }
    parser.parse_args(argc, argv);

    for (auto &input : parser.get<vector<filesystem::path>>("input")) {
      if (input.extension() == ".c") {
        context.sources.push_back(input);
      } else if (input.extension() == ".i") {
        context.preprocessed.push_back(input);
      } else if (input.extension() == ".o") {
        context.objects.push_back(input);
      } else {
        cerr << "Unknown file type: " << input << endl;
        return false;
      }
    }
  } catch (const exception &e) {
    cerr << e.what() << endl;
    cerr << parser << endl;
    return false;
  }

  return true;
}
