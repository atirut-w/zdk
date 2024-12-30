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

  parser->add_argument("filename").nargs(nargs_pattern::at_least_one).action([](const string &value) { return filesystem::path(value); });

  parser->add_argument("-o").action([](const string &value) { return filesystem::path(value); });

  parser->add_argument("-S").flag();

  parser->add_argument("-c", "--compile-only").flag();

  parser->add_argument("-E", "--preprocessonly").flag(); // Someone at SDCC got drunk writing that one

  parser->add_argument("-nostdinc").flag();

  parser->add_argument("-nostdlib").flag();

  auto unknown = parser->parse_known_args(argc, argv);
  return {std::move(parser), unknown};
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

vector<filesystem::path> compile(const vector<filesystem::path> sources, const vector<string> args) {
  vector<filesystem::path> assemblies;
  vector<string> base_args = {"-mz80", "-S"};
  base_args.insert(base_args.end(), args.begin(), args.end());

  for (auto &source : sources) {
    filesystem::path assembly = filesystem::path(source).replace_extension(".s");
    vector<string> args = base_args;
    args.push_back(source.string());
    args.push_back("-o");
    args.push_back(assembly.string());

    int status = run("sdcc-sdcc", args);
    if (status != 0) {
      throw runtime_error("failed to compile " + source.string());
    }

    assemblies.push_back(assembly);
  }

  return assemblies;
}

vector<filesystem::path> assemble(const vector<filesystem::path> sources) {
  vector<filesystem::path> objects;
  vector<string> base_args = {"-sdcc"};

  for (auto &source : sources) {
    filesystem::path object = filesystem::path(source).replace_extension(".o");
    vector<string> args = base_args;
    args.push_back("-o");
    args.push_back(object.string());
    args.push_back(source.string());

    int status = run("z80-elf-as", args);
    if (status != 0) {
      throw runtime_error("failed to assemble " + source.string());
    }

    objects.push_back(object);
  }

  return objects;
}

int main(int argc, char *argv[]) {
  auto [args, unknown] = parse_args(argc, argv);

  vector<filesystem::path> sources;
  vector<filesystem::path> assemblies;
  vector<filesystem::path> objects;
  for (auto &file : args->get<vector<filesystem::path>>("filename")) {
    if (file.extension() == ".c") {
      sources.push_back(file);
    } else if (file.extension() == ".s") {
      assemblies.push_back(file);
    } else if (file.extension() == ".o") {
      objects.push_back(file);
    } else {
      throw runtime_error("unknown file type: " + file.string());
    }
  }

  if (args->get<bool>("-E")) {
    unknown.push_back("-E");
  }
  if (args->get<bool>("-nostdinc")) {
    unknown.push_back("-nostdinc");
  }

  try {
    for (auto &assembly : compile(sources, unknown)) {
      assemblies.push_back(assembly);
    }
  } catch (const runtime_error &e) {
    return 1;
  }

  if (args->get<bool>("-S") || args->get<bool>("-E")) {
    if (args->is_used("-o")) {
      if (assemblies.size() == 1) {
        filesystem::rename(assemblies[0], args->get<filesystem::path>("-o"));
      } else {
        throw runtime_error("cannot use -o with multiple sources");
      }
    }
    return 0;
  }

  try {
    for (auto &object : assemble(assemblies)) {
      objects.push_back(object);
    }
  } catch (const runtime_error &e) {
    return 1;
  }

  if (args->get<bool>("-c")) {
    if (args->is_used("-o")) {
      if (objects.size() == 1) {
        filesystem::rename(objects[0], args->get<filesystem::path>("-o"));
      } else {
        throw runtime_error("cannot use -o with multiple sources");
      }
    }
    return 0;
  }

  vector<string> ld_args = {"-o", "a.out"};
  if (!args->get<bool>("-nostdlib")) {
    ld_args.push_back("-lc");
  }

  for (auto &object : objects) {
    ld_args.push_back(object.string());
  }

  int status = run("z80-elf-ld", ld_args);
  if (status != 0) {
    return status;
  }

  if (args->is_used("-o")) {
    filesystem::rename("a.out", args->get<filesystem::path>("-o"));
  }

  return 0;
}
