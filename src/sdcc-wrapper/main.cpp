#include "argparse/argparse.hpp"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <sched.h>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>
#include <vector>

using namespace std;
using namespace argparse;

pair<unique_ptr<const ArgumentParser>, vector<string>> parse_args(int argc, char *argv[]) {
  auto parser = make_unique<ArgumentParser>("zdk-sdcc", "1.0", default_arguments::none);

  parser->add_argument("filename");
  parser->add_argument("-S");
  parser->add_argument("-c", "--compile-only");
  parser->add_argument("-E", "--preprocessonly"); // Someone at SDCC got drunk writing that one

  auto unknown = parser->parse_known_args(argc, argv);
  return {std::move(parser), std::move(unknown)};
}

int run(string program, vector<string> args) {
  pid_t pid = fork();

  if (pid < 0) {
    throw runtime_error("failed to fork");
  } else if (pid == 0) {
    vector<char *> c_args;
    c_args.push_back(const_cast<char *>(program.data()));
    for (auto &arg : args) {
      c_args.push_back(const_cast<char *>(arg.data()));
    }
    c_args.push_back(nullptr);

    execvp(program.data(), c_args.data());
    _exit(EXIT_FAILURE);
  } else {
    int status;
    waitpid(pid, &status, 0);
    return WEXITSTATUS(status);
  }
}

int main(int argc, char *argv[]) {
  auto [args, unknown] = parse_args(argc, argv);
  filesystem::path filename = args->get<string>("filename");

  vector<string> sdcc_args;
  sdcc_args.push_back("-S");
  sdcc_args.push_back(filename);
  if (argc > 1) {
    sdcc_args.insert(sdcc_args.end(), unknown.begin(), unknown.end());
  }
  run("sdcc-sdcc", sdcc_args);

  if (args->get<bool>("-S")) {
    return 0;
  }

  return 0;
}
