#include <iostream>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <unistd.h>
#include <csignal>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#if defined(__SUNPRO_CC) // sunCC doesn't define kill() in csignal.
#  include <signal.h>
#endif

#include "helpers.h"
#include "exception.h"
#include "pipedexec.h"

PipedExec::PipedExec (void)
{
  this->child_pid = 0; /* Child can not have pid's <= 0. */
  this->child_ret = -1; /* Child can not return values < 0. */
  p2c_pipe[0] = -1;
  p2c_pipe[1] = -1;
  c2p_pipe[0] = -1;
  c2p_pipe[1] = -1;
}

/* ---------------------------------------------------------------- */

PipedExec::~PipedExec (void)
{
  if (p2c_pipe[1] != -1)
    ::close(p2c_pipe[1]);
  if (c2p_pipe[0] != -1)
    ::close(c2p_pipe[0]);
}

/* ---------------------------------------------------------------- */

void
PipedExec::exec (const std::vector<std::string>& cmd)
{
  /* fd[0] is for reading, fd[1] is for writing. */
  if (::pipe(this->p2c_pipe) < 0 || ::pipe(this->c2p_pipe) < 0)
  {
    std::cout << "PipedExec: pipe(): " << ::strerror(errno) << std::endl;
    return;
  }

  pid_t pid = ::fork();
  if (pid < (pid_t)0)
  {
    /* Fork failed. */
    std::cout << "PipedExec: fork(): " << ::strerror(errno) << std::endl;
    ::close(this->p2c_pipe[0]);
    ::close(this->p2c_pipe[1]);
    ::close(this->c2p_pipe[0]);
    ::close(this->c2p_pipe[1]);
    this->child_pid = 0;
    return;
  }

  if (pid)
  {
    /* This is the parent process. */
    ::close(this->p2c_pipe[0]);
    ::close(this->c2p_pipe[1]);
    this->child_pid = pid;

    std::cout << "PID " << pid << " created." << std::endl;
  }
  else
  {
    /* This is the child process. */
    ::close(this->p2c_pipe[1]);
    ::close(this->c2p_pipe[0]);

    ::dup2(this->p2c_pipe[0], STDIN_FILENO);
    ::dup2(this->c2p_pipe[1], STDOUT_FILENO);
    ::close(this->p2c_pipe[0]);
    ::close(this->c2p_pipe[1]);

    char** argv = Helpers::create_argv(cmd);
    ::execvp(argv[0], argv);

    std::string error_msg = "PipedExec: execvp(";
    error_msg += argv[0];
    error_msg += "): ";
    error_msg += ::strerror(errno);
    std::cerr << error_msg << std::endl;

    ::exit(1);
  }
}

/* ---------------------------------------------------------------- */

bool
PipedExec::output_available (void)
{
  fd_set set;
  struct timeval timeout;

  /* Initialize the file descriptor set. */
  FD_ZERO(&set);
  FD_SET(this->c2p_pipe[0], &set);

  /* Initialize the timeout data structure. */
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;

  /* `select' returns 0 if timeout, 1 if input available, -1 if error. */
  int ret = ::select(FD_SETSIZE, &set, NULL, NULL, &timeout);
  return ret == 1;
}

/* ---------------------------------------------------------------- */

std::string
PipedExec::fetch_output (void)
{
  int BUFFER_SIZE = 512;

  std::string ret;
  char* buffer = new char[BUFFER_SIZE];

  while (this->output_available())
  {
    ssize_t new_read = ::read(this->c2p_pipe[0], buffer, BUFFER_SIZE - 1);
    if (new_read == 0) /* EOF */
      break;
    buffer[new_read] = '\0';
    ret += buffer;
  }

  delete[] buffer;
  return ret;
}

/* ---------------------------------------------------------------- */

void
PipedExec::send_data (std::string const& data)
{
  ssize_t size = data.size();
  while (size > 0)
  {
    ssize_t written = ::write(this->p2c_pipe[1], data.c_str(), size);
    if (written < 0)
    {
      std::cout << "Error writing to pipe: " << ::strerror(errno) << std::endl;
      return;
    }
    size -= written;
  }
}

/* ---------------------------------------------------------------- */

bool
PipedExec::has_exited (void)
{
  /* Child status already gathered. */
  if (this->child_ret >= 0)
    return true;

  /* Child process has not yet been launched. */
  if (this->child_pid == 0)
    return false;

  pid_t pid = ::waitpid(this->child_pid, &this->child_ret, WNOHANG);
  if (pid == 0)
    return false;
  else
    return true;
}

/* ---------------------------------------------------------------- */

void
PipedExec::terminate (void)
{
  if (this->child_ret >= 0 || this->child_pid == 0)
    return;

  if (::kill(this->child_pid, SIGTERM) < 0)
    std::cout << ::strerror(errno) << std::endl;
}

/* ---------------------------------------------------------------- */

void
PipedExec::close_sender (void)
{
  ::close(this->p2c_pipe[1]);
  this->p2c_pipe[1] = -1;
}

/* ---------------------------------------------------------------- */

int
PipedExec::waitpid (void)
{
  int status;
  ::waitpid(this->child_pid, &status, 0);
  std::cout << "PID " << this->child_pid
      << " terminated. Status: " << status << std::endl;
  return status;
}
