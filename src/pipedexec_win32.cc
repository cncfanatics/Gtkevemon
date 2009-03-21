#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>
#include <iostream>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <sys/types.h>

#include "helpers.h"
#include "exception.h"
#include "pipedexec.h"

PipedExec::PipedExec (void)
{
  std::cout << "PipedExec::" << __FUNCTION__ << "()" << std::endl;
  this->child_pid = 0; /* Child can not have pid's <= 0. */
  this->child_ret = -1; /* Child can not return values < 0. */
  p2c_pipe[0] = 0;
  p2c_pipe[1] = 0;
  c2p_pipe[0] = 0;
  c2p_pipe[1] = 0;
}

/* ---------------------------------------------------------------- */

PipedExec::~PipedExec (void)
{
  if (p2c_pipe[1] != 0)
    ::CloseHandle(p2c_pipe[1]);
  if (c2p_pipe[0] != 0)
    ::CloseHandle(c2p_pipe[0]);
  ::CloseHandle(this->child_proc);
  ::CloseHandle(this->child_thr);
}

/* ---------------------------------------------------------------- */

void
PipedExec::exec (const std::vector<std::string>& cmd)
{
  std::cout << "PipedExec::" << __FUNCTION__ << "()" << std::endl;
  // Set the bInheritHandle flag so pipe handles are inherited.
  SECURITY_ATTRIBUTES sa;
  sa.nLength = sizeof(sa);
  sa.bInheritHandle = TRUE;
  sa.lpSecurityDescriptor = NULL;

  std::cout << "DEBUG: CreatePipe()" << std::endl;
  /* fd[0] is for reading, fd[1] is for writing. */
  if ( !::CreatePipe(&this->p2c_pipe[0], &this->p2c_pipe[1], &sa, 0)
    || !::CreatePipe(&this->c2p_pipe[0], &this->c2p_pipe[1], &sa, 0))
  {
    std::cerr << "PipedExec: CreatePipe(): " << ::GetLastError() << std::endl;
    ::CloseHandle(this->p2c_pipe[0]);
    ::CloseHandle(this->p2c_pipe[1]);
    ::CloseHandle(this->c2p_pipe[0]);
    ::CloseHandle(this->c2p_pipe[1]);
    return;
  }

  std::cout << "DEBUG: SetHandleInformation()" << std::endl;
  // Ensure the read handle to the pipe for STDOUT is not inherited.
  if (!::SetHandleInformation(this->p2c_pipe[1], HANDLE_FLAG_INHERIT, 0)
      || !::SetHandleInformation(this->c2p_pipe[0], HANDLE_FLAG_INHERIT, 0))
  {
    std::cerr << "PipedExec: SetHandleInformation(): "
        << ::GetLastError() << std::endl;
    ::CloseHandle(this->p2c_pipe[0]);
    ::CloseHandle(this->p2c_pipe[1]);
    ::CloseHandle(this->c2p_pipe[0]);
    ::CloseHandle(this->c2p_pipe[1]);
    return;
  }

  PROCESS_INFORMATION proc_info;
  STARTUPINFO si;
  BOOL bSuccess = FALSE;

  // Set up members of the PROCESS_INFORMATION structure.
  ::memset(&proc_info, 0, sizeof(proc_info));

  // Set up members of the STARTUPINFO structure.
  // This structure specifies the STDIN and STDOUT handles for redirection.
  ::memset(&si, 0, sizeof(si));
  si.cb = sizeof(STARTUPINFO); 
  si.hStdError = this->c2p_pipe[1];
  si.hStdOutput = this->c2p_pipe[1];
  si.hStdInput = this->p2c_pipe[0];
  si.dwFlags |= STARTF_USESTDHANDLES;

  // Create the child process.
  // Must merge argv[] to a single string. There is no other way to pass
  // arguments to CreateProcess().
  std::string tmp = cmd[0];
  for (unsigned int i = 1; i < cmd.size(); i++)
  {
    tmp += " " + cmd[i];
  }
  std::cout << "DEBUG: tmp = \"" << tmp << "\"" << std::endl;
  bSuccess = ::CreateProcess(0,
      (LPSTR)tmp.c_str(), // command line
      0,           // process security attributes
      0,           // primary thread security attributes
      TRUE,        // handles are inherited
      0,           // creation flags
      0,           // use parent's environment
      0,           // use parent's current directory
      &si,         // STARTUPINFO pointer
      &proc_info); // receives PROCESS_INFORMATION

  if (!bSuccess)
  {
    std::cerr << "PipedExec: CreateProcess(): "
        << ::GetLastError() << std::endl;
    ::CloseHandle(this->p2c_pipe[0]);
    ::CloseHandle(this->p2c_pipe[1]);
    ::CloseHandle(this->c2p_pipe[0]);
    ::CloseHandle(this->c2p_pipe[1]);
    return;
  }

  ::CloseHandle(this->p2c_pipe[0]);
  ::CloseHandle(this->c2p_pipe[1]);
  this->child_pid = proc_info.dwProcessId;
  std::cout << "PID " << this->child_pid << " created." << std::endl;

  // Close handles to the child process and its primary thread.
  // Some applications might keep these handles to monitor the status
  // of the child process, for example.
  this->child_proc = proc_info.hProcess;
  this->child_thr = proc_info.hThread;
}

/* ---------------------------------------------------------------- */

bool
PipedExec::output_available (void)
{
  DWORD ret = ::WaitForSingleObject(this->c2p_pipe[0], 0);
  return ret == WAIT_OBJECT_0;
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
    DWORD new_read;
    if (!::ReadFile(this->c2p_pipe[0], buffer, BUFFER_SIZE - 1, &new_read, 0))
    {
      DWORD status = ::GetLastError();
      if (status == ERROR_HANDLE_EOF) /* EOF */
        break;
      else
      {
        std::cerr << "PipedExec: ReadFile(): " << ::GetLastError() << std::endl;
      }
    }
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
  DWORD written;
  if (!::WriteFile(this->p2c_pipe[1], data.c_str(), data.size(), &written, 0))
  {
    std::cout << "Error writing to pipe: " << ::GetLastError() << std::endl;
    return;
  }

  if (written != data.size())
  {
    std::cout << "Warning: Some data could not be send" << std::endl;
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

  DWORD status = ::WaitForSingleObject(this->child_proc, 0);
  if (status == WAIT_TIMEOUT)
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

  if (!::TerminateProcess(this->child_proc, 0))
    std::cout << "PipedExec: TerminateProcess(): "
        << ::GetLastError() << std::endl;

  ::CloseHandle(this->child_proc);
  ::CloseHandle(this->child_thr);
}

/* ---------------------------------------------------------------- */

void
PipedExec::close_sender (void)
{
  std::cout << "DEBUG: PipedExec::" << __FUNCTION__ << "()" << std::endl;
  ::CloseHandle(this->p2c_pipe[1]);
  this->p2c_pipe[1] = 0;
}

/* ---------------------------------------------------------------- */

int
PipedExec::waitpid (void)
{
  DWORD status;
  ::WaitForSingleObject(this->child_proc, INFINITE);
  ::GetExitCodeProcess(this->child_proc, &status);
  std::cout << "PID " << this->child_pid
      << " terminated. Status: " << status << std::endl;
  return status;
}

