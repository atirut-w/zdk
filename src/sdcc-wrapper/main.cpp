#include "argparse/argparse.hpp"
#include <cstdlib>
#include <filesystem>
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

pair<unique_ptr<const ArgumentParser>, vector<string>>
parse_args(int argc, char *argv[]) {
  auto parser =
      make_unique<ArgumentParser>("zdk-sdcc", "1.0", default_arguments::none);

  parser->add_argument("filename");
  parser->add_argument("-S").flag();
  parser->add_argument("-c", "--compile-only").flag();
  parser->add_argument("-E", "--preprocessonly")
      .flag(); // Someone at SDCC got drunk writing that one

  auto unknown = parser->parse_known_args(argc, argv);
  return {std::move(parser), std::move(unknown)};
}

int run(string program, vector<string> args) {
  pid_t pid = fork();

  if (pid < 0) {
    throw runtime_error("failed to fork");
  } else if (pid == 0) {
    vector<char *> c_args;
    c_args.push_back(const_cast<char *>(program.c_str()));
    for (auto &arg : args) {
      c_args.push_back(const_cast<char *>(arg.c_str()));
    }
    c_args.push_back(nullptr);

    execvp(program.c_str(), c_args.data());
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

  vector<string> sdcc_args = {"-mz80", "-S", filename};
  if (args->get<bool>("-E")) {
    sdcc_args.push_back("-E");
  }

  if (argc > 1) {
    sdcc_args.insert(sdcc_args.end(), unknown.begin(), unknown.end());
  }
  int status = run("sdcc-sdcc", sdcc_args);
  if (status != 0) {
    return status;
  }

  if (args->get<bool>("-S") || args->get<bool>("-E")) {
    return 0;
  }

  system(("z80-elf-as -sdcc " + filename.replace_extension(".asm").string() +
          " -o " + filename.replace_extension(".o").string())
             .c_str());
  filesystem::remove(filename.replace_extension(".asm"));

  if (args->get<bool>("-c")) {
    return 0;
  }

  system(
      ("z80-elf-ld " + filename.replace_extension(".o").string() + " -o a.out")
          .c_str());
  filesystem::remove(filename.replace_extension(".o"));

  return 0;
}
