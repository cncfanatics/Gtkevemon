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

HttpDocPtr
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

  /* Read reply. */
  HttpDocPtr doc = HttpDocPtr(new std::string);
  while (true)
  {
    char buffer[512];
    nbytes = ::read(sock, buffer, 511);

    if (nbytes < 0)
    {
      ::close(sock);
      throw Exception(std::string("read() failed: ") + ::strerror(errno));
    }

    if (nbytes == 0)
      break;

    buffer[nbytes] = '\0';
    doc->append(buffer, nbytes);
  }

  ::close(sock);

  //std::cout << "Reply: " << *doc << std::endl;
  //std::cout << "Reply size: " << doc->size() << std::endl;

  this->strip_headers(doc);

  return doc;
}

/* ---------------------------------------------------------------- */

void
Http::strip_headers (HttpDocPtr doc)
{
  /* Find location between header and content. */
  bool dos_nl = false;
  size_t pos = doc->find("\n\n");
  if (pos == std::string::npos)
  {
    pos = doc->find("\r\n\r\n");
    if (pos == std::string::npos)
      return;
    else
      dos_nl = true;
  }

  /* Separate header and content. */
  std::string headers = doc->substr(0, pos + (dos_nl ? 2 : 1));
  *doc = doc->substr(pos + (dos_nl ? 4 : 2));


  //std::cout << "--- Headers ---" << std::endl << headers;
  //std::cout << "--- Document ---" << std::endl << *doc;

  /* Care about some headers. */
  if (headers.find("Transfer-Encoding: chunked") != std::string::npos)
  {
    this->remove_chunks(doc);
  }
}

/* ---------------------------------------------------------------- */

void
Http::remove_chunks (HttpDocPtr doc)
{
  unsigned int read = 0;
  unsigned int write = 0;
  bool read_chunk = true;
  std::string chunk_size_str;
  unsigned int chunk_size = 0;
  unsigned int chunk_counter = 0;

  while (read < doc->size())
  {
    if (read_chunk)
    {
      char c = doc->at(read);
      if (c == '\r')
      {
      }
      else if (c == '\n')
      {
        chunk_size = this->get_int_from_hex(chunk_size_str);
        if (chunk_size == 0)
          break;
        chunk_size_str.clear();
        read_chunk = false;
        chunk_counter = 0;
      }
      else
      {
        chunk_size_str += c;
      }

      read += 1;
    }
    else
    {
      doc->at(write) = doc->at(read);
      write += 1;
      read += 1;
      chunk_counter += 1;
      if (chunk_counter >= chunk_size)
      {
        read_chunk = true;
      }
    }
  }

  doc->resize(write);
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
