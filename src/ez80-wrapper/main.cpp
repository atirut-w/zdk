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

pair<unique_ptr<const ArgumentParser>, vector<string>>
parse_args(int argc, char *argv[]) {
  auto parser =
      make_unique<ArgumentParser>("zdk-sdcc", "1.0", default_arguments::none);

  parser->add_argument("filename");
  parser->add_argument("-o").action(
      [](const string &value) { return filesystem::path(value); });
  parser->add_argument("-S").flag();
  parser->add_argument("-c", "--compile-only").flag();
  parser->add_argument("-E", "--preprocessonly")
      .flag(); // Someone at SDCC got drunk writing that one

  auto unknown = parser->parse_known_args(argc, argv);
  return {std::move(parser), std::move(unknown)};
}

int run(string program, vector<string> args) {
  cout << "# " << program << " ";
  for (auto &arg : args) {
    cout << arg << " ";
  }
  cout << endl;

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
  filesystem::path intermediate = filename;
  int status;

  vector<string> clang_args = {"-target", "z80-unknown-elf", "-S", filename.string(), "-o", intermediate.replace_extension(".asm")};
  if (args->get<bool>("-E")) {
    clang_args.push_back("-E");
  }
  if (argc > 1) {
    clang_args.insert(clang_args.end(), unknown.begin(), unknown.end());
  }
  
  status = run("ez80-clang", clang_args);
  if (status != 0) {
    return status;
  }

  if (args->get<bool>("-S") || args->get<bool>("-E")) {
    if (args->is_used("-o")) {
      filesystem::rename(intermediate, args->get<filesystem::path>("-o"));
    }
    return 0;
  }

  status = run("z80-elf-as", {"-sdcc", "-o", intermediate.replace_extension(".o"), intermediate.replace_extension(".asm")});
  if (status != 0) {
    return status;
  }

  filesystem::remove(intermediate.replace_extension(".asm"));
  if (args->get<bool>("-c")) {
    if (args->is_used("-o")) {
      filesystem::rename(intermediate.replace_extension(".o"), args->get<filesystem::path>("-o"));
    }
    return 0;
  }

  status = run("z80-elf-ld", {"-o", "a.out", intermediate.replace_extension(".o")});
  if (status != 0) {
    return status;
  }

  filesystem::remove(intermediate.replace_extension(".o"));
  if (args->is_used("-o")) {
    filesystem::rename("a.out", args->get<filesystem::path>("-o"));
  }

  return 0;
}
