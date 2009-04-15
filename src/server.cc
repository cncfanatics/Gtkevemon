#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <csignal>
#include <cerrno>
#include <cstring>
#include <iostream>

#include "exception.h"
#include "nettcpsocket.h"
#include "server.h"

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

  Net::TCPSocket sock;
  try
  {
    /* Connect and see if it's online or not. */
    sock.set_connect_timeout(SERVER_TIMEOUT * 1000);
    sock.connect(this->host, this->port);
  }
  catch (Exception& e)
  {
    /* Nope. Not online or some error occured. */
    sock.close();
    this->refreshing = false;
    this->online = false;
    this->players = 0;

    std::cout << "Server info: " << this->name
        << " offline. " << e << std::endl;

    return;
  }

  /* Cool. Online. */
  this->online = true;

  /* Try to get the amount of players. */
  unsigned char buffer[SERVER_READ_BYTES + 1];

  try
  {
    std::size_t nbytes = sock.full_read(buffer, SERVER_READ_BYTES);
    buffer[SERVER_READ_BYTES] = '\0';

    if (nbytes < SERVER_READ_BYTES)
      throw Exception("Server protocol not recognized");
  }
  catch (Exception& e)
  {
    sock.close();
    this->refreshing = false;
    this->players = -2;

    std::cout << "Server info: " << this->name << " online. "
        << "Players: Unknown (" << e << ")" << std::endl;

    return;
  }

  sock.close();

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

  this->refreshing = false;

  std::cout << "Server info: " << this->name << " online. "
      << "Players: " << this->players << std::endl;
}
