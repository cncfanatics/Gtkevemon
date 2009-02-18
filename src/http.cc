#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cmath>
#include <cerrno>
#include <cstring>
#include <cstdlib>
//#include <iostream>
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
        + std::string(::hstrerror(herr)));

#elif defined(__APPLE__)

  /* Under MacOS X the gethostbyname function is already thread safe. */
  this->result = *::gethostbyname(host);

#elif defined(__SunOS)

  /* Solaris has 1 argument fewer than Linux. */
  struct hostent* res;
  int herr;

  /* Define initial buffer length and alloc space. */
  size_t buffer_len = 1024;
  this->strange_data = (char*)::malloc(buffer_len);

  do
  {
    res = ::gethostbyname_r(host, &this->result,
        this->strange_data, buffer_len, &herr);
    /*  Check for errors.  */
    if (res == 0 && herr == ERANGE)
    {
      /* Enlarge the buffer.  */
      buffer_len *= 2;
      this->strange_data = (char*)::realloc(this->strange_data, buffer_len);
    }
  }
  while (res == 0 && herr == ERANGE);

  if (res == 0 && herr != 0)
    throw Exception("gethostbyname_r() failed: "
        + std::string(::hstrerror(herr)));

#else

  /* There is no solution for other systems yet. */
# error "gethostbyname_r(): no implementation available. PLEASE REPORT THIS!"

#endif
}

GetHostByName::~GetHostByName (void)
{
  ::free(this->strange_data);
}

/* ================================================================ */

Http::Http (void)
{
  this->initialize_defaults();
}

/* ---------------------------------------------------------------- */

Http::Http (std::string const& url)
{
  this->set_url(url);
  this->initialize_defaults();
}

/* ---------------------------------------------------------------- */

Http::Http (std::string const& host, std::string const& path)
{
  this->host = host;
  this->path = path;
  this->initialize_defaults();
}

/* ---------------------------------------------------------------- */

void
Http::initialize_defaults (void)
{
  this->method = HTTP_METHOD_GET;
  this->port = 80;
  this->agent = "VerySimpleHttpRequester";
  this->proxy_port = 80;
  this->http_state = HTTP_STATE_READY;
  this->bytes_read = 0;
  this->bytes_total = 0;
}

/* ---------------------------------------------------------------- */

void
Http::set_url (std::string const& url)
{
  if (url.size() <= 7)
    throw Exception("Invalid URL");

  if (url.substr(0, 7) != "http://")
    throw Exception("Invalid protocol");

  std::string host = url.substr(7);
  size_t spos = host.find_first_of('/');
  if (spos == std::string::npos)
    throw Exception("No path specification");

  if (spos == 0)
    throw Exception("Invalid host specification");

  std::string path = host.substr(spos);
  host = host.substr(0, spos);

  //std::cout << "Setting host and path: " << host << ":" << path << std::endl;
  this->host = host;
  this->path = path;
}

/* ---------------------------------------------------------------- */

HttpDataPtr
Http::request (void)
{
  this->bytes_read = 0;
  this->bytes_total = 0;

  int sock = -1;
  try
  {
    /* Initialize a valid HTTP connection. */
    this->http_state = HTTP_STATE_CONNECTING;
    sock = this->initialize_connection();

    /* Successful connect. Start building the request. */
    this->http_state = HTTP_STATE_REQUESTING;
    this->send_http_headers(sock);

    /* Request has been sent. Read reply. */
    this->http_state = HTTP_STATE_RECEIVING;
    HttpDataPtr result = this->read_http_reply(sock);

    /* Done reading the reply. */
    this->http_state = HTTP_STATE_DONE;
    ::close(sock);
    return result;
  }
  catch (Exception& e)
  {
    this->http_state = HTTP_STATE_ERROR;
    if (sock >= 0)
      ::close(sock);
    throw e;
  }
}

/* ---------------------------------------------------------------- */

/* Initializes a valid HTTP connection or throws an exception. */
int
Http::initialize_connection (void)
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
      remote.sin_family = AF_INET;
      remote.sin_port = htons(this->port);
      remote.sin_addr.s_addr = *(uint32_t*)hn.result.h_addr;
    }
    catch (Exception& e)
    {
      ::close(sock);
      throw Exception(e);
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

  return sock;
}

/* ---------------------------------------------------------------- */

void
Http::send_http_headers (int sock)
{
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
    throw Exception(std::string("write() failed: ") + ::strerror(errno));
}

/* ---------------------------------------------------------------- */

HttpDataPtr
Http::read_http_reply (int sock)
{
  HttpDataPtr result = HttpData::create();
  bool chunked_read = false;

  /* Read the headers. */
  while (true)
  {
    std::string line;
    int ret = this->socket_read_line(sock, line);

    /* Exit loop if we read an empty line. */
    if (ret == 0)
      break;

    result->headers.push_back(line);
    if (line == "Transfer-Encoding: chunked")
      chunked_read = true;
    else if (line.substr(0, 16) == "Content-Length: ")
      this->bytes_total = (size_t)this->get_uint_from_str(line.substr(16));
  }

  //std::cout << "Dump of HTTP reply headers" << std::endl;
  //for (unsigned int i = 0; i < result->headers.size(); ++i)
  //  std::cout << "  " << result->headers[i] << std::endl;

  if (chunked_read)
  {
    /* Deal with chunks *sigh*. */
    unsigned int size = 512;
    unsigned int pos = 0;
    result->data.resize(size);

    while (true)
    {
      /* Read line with chunk size information. */
      std::string line;
      int ret = this->socket_read_line(sock, line);
      if (ret == 0)
        break;

      /* Convert to number of bytes to be read. */
      unsigned int chunk_size = this->get_uint_from_hex(line);
      if (chunk_size == 0)
        break;

      /* Grow buffer if we run out of space. */
      while (pos + chunk_size >= size)
      {
        size *= 2;
        result->data.resize(size, '\0');
      }

      /* Read bytes. */
      ssize_t nbytes = this->http_data_read(sock,
          &result->data[pos], chunk_size);
      pos += nbytes;
      if (nbytes != (ssize_t)chunk_size)
        break;
    }

    result->data.resize(pos);
  }
  else if (this->bytes_total == 0)
  {
    /* The Server did not specify a content-length header. */
    unsigned int size = 512;
    unsigned int pos = 0;

    while (true)
    {
      if (result->data.size() < pos + size)
        result->data.resize(pos + size, '\0');

      ssize_t nbytes = this->http_data_read(sock, &result->data[pos], size);
      if (nbytes == 0)
        break;
      pos += nbytes;
    }
    result->data.resize(pos + 1, '\0');
  }
  else
  {
    /* Simply copy the buffer to the result. Allocating one more byte and
     * setting the memory to '\0' makes it safe for use as string.
     * WARNING: For binary data you should always remove the last character.
     */
    result->data.resize(this->bytes_total + 1);
    ::memset(&result->data[0], '\0', this->bytes_total + 1);
    this->http_data_read(sock, &result->data[0], this->bytes_total);
  }

  return result;
}

/* ---------------------------------------------------------------- */

unsigned int
Http::get_uint_from_hex (std::string const& str)
{
  if (str.empty())
    return 0;

  unsigned int v = 0;
  unsigned int mult = 1;

  for (int i = str.size() - 1; i >= 0; --i)
  {
    switch (str[i])
    {
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
      default: throw Exception("Invalid HEX character");
    }
    mult = mult * 16;
  }

  return v;
}

/* ---------------------------------------------------------------- */

unsigned int
Http::get_uint_from_str (std::string const& str)
{
  std::stringstream ss(str);
  int ret;
  ss >> ret;
  return ret;
}

/* ---------------------------------------------------------------- */

ssize_t
Http::socket_read_line (int sock, std::string& line)
{
  ssize_t i = 0;
  /* whether the previous char was a \r */
  bool cr = false;
  char ch;

  while (true)
  {
    /* Read a character. */
    ssize_t ret = ::read(sock, &ch, 1);
    if (ret < 0)
      throw Exception(::strerror(errno));
    else if (ret == 0)
      break;

    /* Check for newlines. */
    if (ch == '\n')
    {
      /* If preceded by a \r, remove it. */
      if (cr)
      {
        line.erase(line.size() - 1);
        i -= 1;
      }
      break;
    }
    else
    {
      cr = (ch == '\r');
      line.push_back(ch);
    }

    i += 1;
  }

  return i;
}

/* ---------------------------------------------------------------- */

ssize_t
Http::http_data_read (int sock, char* buf, size_t size)
{
  ssize_t ret = 0;
  while (ret < (ssize_t)size)
  {
    ssize_t new_read = ::read(sock, buf + ret, size - ret);
    if (new_read < 0) /* Error */
      throw Exception(::strerror(errno));
    else if (new_read == 0) /* EOF */
      break;

    ret += new_read;
    this->bytes_read += new_read;
  }

  return ret;
}
