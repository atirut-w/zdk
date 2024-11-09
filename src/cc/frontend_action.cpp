#include "frontend_action.hpp"
#include <stdexcept>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int FrontendAction::run_command(const std::string &program, const std::vector<std::string> &args) {
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
