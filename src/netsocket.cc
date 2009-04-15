#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <cerrno>
#include <cstring>

#include "exception.h"
#include "netsocket.h"

NET_NAMESPACE_BEGIN

Socket::Socket (void)
{
  this->sock = NET_INVALID_SOCKET;
}

/* ---------------------------------------------------------------- */

Socket::Socket (NetSocketFD sock_fd)
{
  this->sock = sock_fd;
}

/* ---------------------------------------------------------------- */

Socket::~Socket (void)
{
}

/* ---------------------------------------------------------------- */

void
Socket::close (void)
{
#ifndef WIN32
  ::close(this->sock);
#else
  ::closesocket(this->sock);
#endif
  this->sock = NET_INVALID_SOCKET;
}

/* ---------------------------------------------------------------- */

void
Socket::shutdown_input (void)
{
#ifndef WIN32
  if (::shutdown(this->sock, SHUT_RD) < 0)
    throw Exception(::strerror(errno));
#else
  if (::shutdown(this->sock, SD_RECEIVE) == SOCKET_ERROR)
    throw Exception(::wsa_strerror(::WSAGetLastError()));
#endif
}

/* ---------------------------------------------------------------- */

void
Socket::shutdown_output (void)
{
#ifndef WIN32
  if (::shutdown(this->sock, SHUT_WR) < 0)
    throw Exception(::strerror(errno));
#else
  if (::shutdown(this->sock, SD_SEND) == SOCKET_ERROR)
    throw Exception(::wsa_strerror(::WSAGetLastError()));
#endif
}

/* ---------------------------------------------------------------- */

bool
Socket::is_connected (void) const
{
  return this->sock != NET_INVALID_SOCKET;
}

/* ---------------------------------------------------------------- */

bool
Socket::input_available (void) const
{
  fd_set set;
  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;

  FD_ZERO(&set);
  FD_SET(this->sock, &set);

  /* select returns 0 if timeout, 1 if input available, -1 if error. */
  return ::select(FD_SETSIZE, &set, NULL, NULL, &timeout) == 1;
}

/* ---------------------------------------------------------------- */

std::size_t
Socket::read (void* buffer, std::size_t size, std::size_t offset)
{
#ifndef WIN32
  ssize_t ret = ::recv(this->sock, (char*)buffer + offset, size, 0);
  if (ret < 0)
    throw Exception(::strerror(errno));
#else
  int ret = ::recv(this->sock, (char*)buffer + offset, size, 0);
  if (ret == SOCKET_ERROR)
    throw Exception(::wsa_strerror(::WSAGetLastError()));
#endif

  return ret;
}

/* ---------------------------------------------------------------- */

std::size_t
Socket::full_read (void* buffer, std::size_t size, std::size_t offset)
{
  std::size_t ret = 0;
  while (ret < size)
  {
    ssize_t new_read = this->read(buffer, size - ret, ret + offset);
    if (new_read == 0) /* EOF */
      break;

    ret += new_read;
  }

  return ret;
}

/* ---------------------------------------------------------------- */

void
Socket::read_line (std::string& result, std::size_t max_len)
{
  char buf[1];

  while (result.length() < 1 || result[result.size() - 1] != '\n')
  {
    ssize_t new_read = this->read(buf, 1, 0);
    if (new_read == 0) /* EOF */
      break;

    result.push_back(buf[0]);
    if (max_len != 0 && result.size() >= max_len)
      break;
  }

  while (!result.empty() && (result[result.size() - 1] == '\n'
      || result[result.size() - 1] == '\r'))
    result.erase(result.end() - 1);
}

/* ---------------------------------------------------------------- */

std::size_t
Socket::write (void const* buffer, std::size_t size, std::size_t offset)
{
  char const* buf = (char const*)buffer;
#ifndef WIN32
  ssize_t ret = ::send(this->sock, buf + offset, size, 0);
  if (ret < 0)
    throw Exception(::strerror(errno));
#else
  int ret = ::send(this->sock, buf + offset, size, 0);
  if (ret == SOCKET_ERROR)
    throw Exception(::wsa_strerror(::WSAGetLastError()));
#endif

  return ret;
}

/* ---------------------------------------------------------------- */

std::size_t
Socket::full_write (void const* buffer, std::size_t size, std::size_t offset)
{
  std::size_t ret = 0;
  while (ret < size)
  {
    std::size_t new_write = this->write(buffer, size - ret, ret + offset);
    if (new_write == 0)
      throw Exception("Zero-byte transfer");
    ret += new_write;
  }

  return ret;
}

/* ---------------------------------------------------------------- */

void
Socket::write_str (std::string const& text)
{
  this->full_write(text.c_str(), text.length(), 0);
}

/* ---------------------------------------------------------------- */

void
Socket::write_line (std::string const& text)
{
  this->full_write(text.c_str(), text.length(), 0);
  if (text.length() == 0 || text[text.length() - 1] != '\n')
    this->full_write("\n", 1, 0);
}

/* ---------------------------------------------------------------- */

NetSocketFD
Socket::get_socket_fd (void)
{
  return this->sock;
}

NET_NAMESPACE_END
