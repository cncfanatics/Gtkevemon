#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <csignal>
#include <cerrno>
#include <cstring>
#include <iostream>

#include "exception.h"
#include "server.h"

int Server::current_socket = 0;

/* ---------------------------------------------------------------- */

Server::Server (std::string const& name, std::string const& host, uint16_t port)
{
  this->name = name;
  this->host = host;
  this->port = port;

  this->refreshing = false;
  this->online = false;
  this->players = -1;
}

/* ---------------------------------------------------------------- */

Server::~Server (void)
{
}

/* ---------------------------------------------------------------- */

void
Server::refresh (void)
{
  try
  {
    this->refresh_intern();
    this->sig_updated.emit();
  }
  catch (Exception& e)
  {
    this->sig_updated.emit();
    throw e;
  }
}

/* ---------------------------------------------------------------- */

void
Server::refresh_intern (void)
{
  this->refreshing = true;
  this->online = false;
  this->players = 0;

  /* Create socket. */
  int sock = ::socket(PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    this->refreshing = false;
    throw Exception(std::string("socket() failed: ") + ::strerror(errno));
  }

  /* Prepare for internet. */
  struct sockaddr_in remote;

  remote.sin_family = AF_INET;
  remote.sin_port = htons(this->port);

  /* Set host and port. */
  if (::inet_aton(this->host.c_str(), &remote.sin_addr) == 0)
  {
    this->refreshing = false;
    ::close(sock);
    throw Exception(std::string("inet_aton() failed: ") + ::strerror(errno));
  }

  /* Set timeout handler. */
  Server::current_socket = sock;
  ::signal(SIGALRM, Server::alarm_expired);
  ::alarm(SERVER_TIMEOUT);

  //std::cout << "Connecting to " << this->name << "..." << std::endl;

  /* Connect and see if it's online or not. */
  if (::connect(sock, (struct sockaddr*)&remote,
      sizeof(struct sockaddr_in)) < 0)
  {
    this->refreshing = false;
    this->online = false;
    this->players = 0;
    ::alarm(0);
    Server::current_socket = 0;
    ::close(sock);

    std::cout << "Server info: " << this->name << " offline. "
        << ::strerror(errno) << std::endl;

    return;
  }

  /* Deactivate timer. */
  ::alarm(0);
  Server::current_socket = 0;

  /* Cool. Online. */
  this->online = true;

  /* Try to get the amount of players. */
  unsigned char buffer[SERVER_READ_BYTES + 1];
  size_t offset = 0;

  while (offset < SERVER_READ_BYTES)
  {
    ssize_t ret = ::read(sock, buffer + offset, SERVER_READ_BYTES - offset);
    offset += ret;

    if (ret < 0)
    {
      this->refreshing = false;
      ::close(sock);
      throw Exception(std::string("read() failed: ") + ::strerror(errno));
    }

    if (ret == 0)
      break;
  }

  buffer[SERVER_READ_BYTES] = '\0';

  // Amended usercount checks, info from clef on iRC
  // [16:01] <clef> BradStone: for the moment, take that byte[19]
  //         ... if it is 1, 8 or 9, the usercount is 0.
  // [16:01] <clef> BradStone: if it is 4, the next 32bit are the
  //         ... usercount. 5 -> 16bit. 6 -> 8bit.
  switch (buffer[19])
  {
    case 4:
      this->players = (int)buffer[20] + ((int)buffer[21] << 8)
          + ((int)buffer[22] << 16) + ((int)buffer[23] << 24);
      //std::cout << "Player field is 32 bit: " << this->players << std::endl;
      break;
    case 5:
      this->players = (int)buffer[20] + ((int)buffer[21] << 8);
      //std::cout << "Player field is 16 bit: " << this->players << std::endl;
      break;
    case 6:
      this->players = (int)buffer[20];
      //std::cout << "Player field is 8 bit: " << this->players << std::endl;
      break;
    default:
      this->players = 0;
  }

  ::close(sock);
  this->refreshing = false;

  std::cout << "Server info: " << this->name << " online. "
      << "Players: " << this->players << std::endl;
}

/* ---------------------------------------------------------------- */

void
Server::alarm_expired (int signal)
{
  signal = 0;

  /* Just close the socket, that will connect force to return with error. */
  ::shutdown(Server::current_socket, SHUT_RDWR);
}
