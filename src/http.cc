#include <netdb.h>
#include <arpa/inet.h>
#include <cmath>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

#include "exception.h"
#include "http.h"

Http::Http (void)
{
  this->method = HTTP_METHOD_GET;
  this->port = 80;
  this->agent = "VerySimpleHttpRequester";
  this->proxy_port = 80;
}

/* ---------------------------------------------------------------- */

Http::Http (std::string const& host, std::string const& path)
{
  this->method = HTTP_METHOD_GET;
  this->port = 80;
  this->host = host;
  this->path = path;
  this->agent = "VerySimpleHttpRequester";
  this->proxy_port = 80;
}

/* ---------------------------------------------------------------- */

HttpDataPtr
Http::request (void)
{
  /* Some error checking. */
  if (this->host.size() == 0)
    throw Exception("Internal: Host not set");

  if (this->path.size() == 0)
    throw Exception("Internal: Path not set");

  /* Create socket. */
  int sock = ::socket(PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    throw Exception(std::string("socket() failed: ") + ::strerror(errno));

  /* Prepare for internet. */
  struct sockaddr_in remote;

  if (this->proxy.empty())
  {
    /* The no-proxy-case. Get IP adress for the host. */
    struct hostent* target = gethostbyname(this->host.c_str());
    if (!target)
    {
      ::close(sock);
      throw Exception(std::string("gethostbyname() falied: ")
          + ::strerror(errno));
    }

    char address[INET_ADDRSTRLEN];
    if(::inet_ntop(AF_INET, target->h_addr, address, INET_ADDRSTRLEN) == 0)
    {
      ::close(sock);
      throw Exception(std::string("inet_ntop() failed: ") + ::strerror(errno));
    }

    //std::cout << "Resolved " << this->host << " to " << address << std::endl;

    remote.sin_family = AF_INET;
    remote.sin_port = ::htons(this->port);
    remote.sin_addr.s_addr = *(unsigned long*)target->h_addr;
  }
  else
  {
    /* The proxy case. Assume the proxy is given as IP address. */
    remote.sin_family = AF_INET;
    remote.sin_port = htons(this->proxy_port);

    if (::inet_aton(this->proxy.c_str(), &remote.sin_addr) == 0)
    {
      ::close(sock);
      throw Exception(std::string("inet_aton() failed: ") + ::strerror(errno));
    }
  }

  /* Connect and see if it works. */
  if (::connect(sock, (struct sockaddr*)&remote,
      sizeof(struct sockaddr_in)) < 0)
  {
    ::close(sock);
    throw Exception(std::string("connect() failed: ") + ::strerror(errno));
  }

  std::string request_path;
  if (this->proxy.empty())
  {
    /* Simply use the path. */
    request_path = this->path;
  }
  else
  {
    /* Prefix the host in the proxy case. */
    request_path = "http://" + this->host + this->path;
  }

  /* We are connected. Create the headers. */
  std::stringstream headers;
  if (this->method == HTTP_METHOD_POST)
    headers << "POST " << request_path << " HTTP/1.1\n";
  else
  {
    headers << "GET " << request_path;
    if (this->data.size() > 0)
      headers << "?" << this->data;
    headers << " HTTP/1.1\n";
  }
  headers << "Host: " << this->host << "\n";
  headers << "User-Agent: " << this->agent << "\n";
  headers << "Connection: close\n";

  for (unsigned int i = 0; i < this->headers.size(); ++i)
    headers << this->headers[i] << "\n";

  if (this->method == HTTP_METHOD_POST)
  {
    headers << "Content-Type: application/x-www-form-urlencoded\n";
    headers << "Content-Length: " << this->data.size() << "\n";
    headers << "\n";
    headers << this->data << "\n";
  }
  else
  {
    headers << "\n";
  }

  std::string header_str = headers.str();
  //std::cout << "Sending: " << header_str << std::endl;

  /* Send the headers. */
  ssize_t nbytes = ::write(sock, header_str.c_str(), header_str.size());
  if (nbytes < 0)
  {
    ::close(sock);
    throw Exception(std::string("write() failed: ") + ::strerror(errno));
  }

  /* Read reply into the data array. */
  std::vector<char> data;
  size_t dpos = 0;

  while (true)
  {
    data.resize(dpos + 512, '\0');
    char* buffer = &data[dpos];
    nbytes = ::read(sock, buffer, 512);

    if (nbytes < 0)
    {
      ::close(sock);
      throw Exception(std::string("read() failed: ") + ::strerror(errno));
    }

    if (nbytes == 0)
      break;

    dpos = dpos + nbytes;
  }
  /* Push a trailing \0. */
  data.push_back('\0');
  data[dpos] = '\0';
  dpos++;

  /* Ok, we're done with the connection. */
  ::close(sock);

  /* Create the result, thats the return value. */
  HttpDataPtr result = HttpData::create();

  /* Now strip the headers from the body and copy to result. */
  bool chunked_read = false;
  size_t pos = 0;
  while (true)
  {
    std::string line;
    pos = this->data_readline(line, data, pos);

    if (!line.empty())
    {
      result->headers.push_back(line);

      if (line == "Transfer-Encoding: chunked")
        chunked_read = true;
    }
    else
    {
      /* Empty header line. Content starts now. */
      break;
    }
  }

  if (chunked_read)
  {
    /* Deal with chunks *sigh*. */

    /* Alloc the full size ignoring chunk space, this is a bit to much,
     * but it doesn't care. But the size argument should be right!. */
    result->alloc(dpos - pos);
    result->size = 0;
    while (true)
    {
      std::string line;
      pos = this->data_readline(line, data, pos);
      unsigned int bytes = this->get_int_from_hex(line);
      if (bytes == 0)
        break;
      ::memcpy(result->data, &data[pos], bytes);
      pos += bytes;
      result->size += bytes;
    }
  }
  else
  {
    /* Simply copy the buffer to the result. */
    result->alloc(dpos - pos);
    ::memcpy(result->data, &data[pos], dpos - pos);
  }

  /*
  std::cout << "Received Headers:" << std::endl;
  for (unsigned int i = 0; i < result->headers.size(); ++i)
    std::cout << "  " << result->headers[i] << std::endl;
  std::cout << "Reply size: " << result->size << std::endl;
  */

  return result;
}

/* ---------------------------------------------------------------- */

size_t
Http::data_readline (std::string& dest,
    std::vector<char> const& data, size_t pos)
{
  dest.clear();

  for (size_t iter = pos; true; ++iter)
  {
    if (data[iter] == '\r')
      return iter + 2;

    if (data[iter] == '\n')
      return iter + 1;

    dest.push_back(data[iter]);
  }
}

/* ---------------------------------------------------------------- */

unsigned int
Http::get_int_from_hex (std::string const& str)
{
  if (str.empty())
    return 0;

  unsigned int v = 0;
  unsigned int mult = 1;

  for (int i = str.size() - 1; i >= 0; --i)
  {
    switch (str[i])
    {
      default:
      case '0': break;
      case '1': v += 1 * mult; break;
      case '2': v += 2 * mult; break;
      case '3': v += 3 * mult; break;
      case '4': v += 4 * mult; break;
      case '5': v += 5 * mult; break;
      case '6': v += 6 * mult; break;
      case '7': v += 7 * mult; break;
      case '8': v += 8 * mult; break;
      case '9': v += 9 * mult; break;
      case 'A': case 'a': v += 10 * mult; break;
      case 'B': case 'b': v += 11 * mult; break;
      case 'C': case 'c': v += 12 * mult; break;
      case 'D': case 'd': v += 13 * mult; break;
      case 'E': case 'e': v += 14 * mult; break;
      case 'F': case 'f': v += 15 * mult; break;
    }
    mult = mult * 16;
  }

  return v;
}
