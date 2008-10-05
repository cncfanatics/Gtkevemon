#include "pipedexec.h"
#include "helpers.h"
#include "mailer.h"

int
Mailer::send (std::string const& address, std::string const& subject,
    std::string const& message)
{
  StringVector command;
  command.push_back("mail");
  command.push_back("-s");
  command.push_back(subject);
  command.push_back(address);

  PipedExec pe;
  pe.exec(command);

  pe.send_data(message);
  pe.close_sender();

  return pe.waitpid();
}
