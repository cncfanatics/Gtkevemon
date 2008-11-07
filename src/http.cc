#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cmath>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "exception.h"
#include "http.h"

/* Holds a struct hostent with magical dynamically allocated data.
 * This is thread safe in contrast to the gethostbyname() function.
 */
class GetHostByName
{
  private:
    char* strange_data;

  public:
    struct hostent result;

  public:
    GetHostByName (char const* host);
    ~GetHostByName (void);
};

/* Some comment about this function I found on the internet:
 *
 *   "Oh boy, this interface sucks so badly, there are no words for it.
 *   Not one, not two, but _three_ error signalling methods!  (*h_errnop
 *   nonzero?  return value nonzero?  *RESULT zero?)  The glibc goons
 *   really outdid themselves with this one."
 */
GetHostByName::GetHostByName (char const* host)
{
  this->strange_data = 0;

#if defined(_GNU_SOURCE) || defined(__FreeBSD__)

  /* If this is compiled as GNU source, assume that
   * gethostbyname_r is available. */
  struct hostent* hp;
  int res;
  int herr;

  /* Define initial buffer length and alloc space. */
  size_t buffer_len = 1024;
  this->strange_data = (char*)::malloc(buffer_len);

  while ((res = ::gethostbyname_r(host, &this->result,
      this->strange_data, buffer_len, &hp, &herr)) == ERANGE)
  {
    /* Enlarge the buffer.  */
    buffer_len *= 2;
    this->strange_data = (char*)::realloc(this->strange_data, buffer_len);
  }

  /*  Check for errors.  */
  if (res || hp == 0)
    throw Exception("gethostbyname_r() failed: "
        + std::string(::strerror(errno)));

#else
#  ifdef __APPLE__

  /* Under MacOS X the gethostbyname function is already thread safe. */
  this->result = *::gethostbyname(host);

#  else

  /* There is no solution for other systems yet. */
#    error "gethostbyname_r(): no implementation available. PEASE REPORT THIS!"

#  endif
#endif
}

GetHostByName::~GetHostByName (void)
{
  ::free(this->strange_data);
}

/* ---------------------------------------------------------------- */

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
    try
    {
      GetHostByName hn(this->host.c_str());

      //char address[INET_ADDRSTRLEN];
      //if(!::inet_ntop(AF_INET, hn.result.h_addr, address, INET_ADDRSTRLEN))
      //{
      //  ::close(sock);
      //  throw Exception(std::string("inet_ntop() failed: ")
      //      + ::strerror(errno));
      //}
      //std::cout << "Resolved " << this->host << ": " << address << std::endl;

      remote.sin_family = AF_INET;
      remote.sin_port = htons(this->port);
      remote.sin_addr.s_addr = *(uint32_t*)hn.result.h_addr;
    }
    catch (Exception& e)
    {
      ::close(sock);
      throw e;
    }
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

  /* Append additional headers specified from the outside. */
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
     * but it doesn't matter. But the size member is still precise!. */
    result->alloc(dpos - pos + 1);
    result->size = 0;
    ::memset(result->data, '\0', dpos - pos + 1);
    while (true)
    {
      std::string line;
      pos = this->data_readline(line, data, pos);
      unsigned int bytes = this->get_int_from_hex(line);
      if (bytes == 0)
        break;
      ::memcpy(&result->data[result->size], &data[pos], bytes);
      pos += bytes;
      result->size += bytes;
    }
  }
  else
  {
    /* Simply copy the buffer to the result. Allocating one more byte and
     * setting the memory to '\0' makes it safe for use as string. But
     * the size member is still precise and does not include the '\0'. */
    result->alloc(dpos - pos + 1);
    result->size = dpos - pos;
    ::memset(result->data, '\0', dpos - pos + 1);
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
    if (iter >= data.size())
      return iter;

    if (data[iter] == '\0')
      return iter;

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
