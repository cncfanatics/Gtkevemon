#ifndef HTTP_HEADER
#define HTTP_HEADER

#include <vector>
#include <string>
#include "ref_ptr.h"

enum HttpMethod
{
  HTTP_METHOD_GET,
  HTTP_METHOD_POST
};

/* ---------------------------------------------------------------- */

/*
class HttpData;
typedef ref_ptr<HttpData> HttpDataPtr;

class HttpData
{
  protected:
    HttpData (void)
    { this->data = 0; }

    HttpData (size_t size)
    { this->data = new char[size]; }

  public:
    std::string headers;
    char* data;

  public:
    static HttpDataPtr create (void)
    { return HttpDataPtr(new HttpData); }

    static HttpDataPtr create (size_t size)
    { return HttpDataPtr(new HttpData(size)); }

    ~HttpData (void)
    { delete [] this->data; }
};
*/

typedef ref_ptr<std::string> HttpDocPtr;

/* ---------------------------------------------------------------- */

/* Class for very simple requesting of documents over HTTP. */
class Http
{
  private:
    HttpMethod method;
    std::string data;
    std::string host;
    std::string path;
    std::string agent;
    unsigned int port;
    std::vector<std::string> headers;

    std::string proxy;
    unsigned int proxy_port;

    void strip_headers (HttpDocPtr doc);
    void remove_chunks (HttpDocPtr doc);
    unsigned int get_int_from_hex (std::string const& str);

  public:
    Http (void);
    Http (std::string const& host, std::string const& path);

    void set_data (HttpMethod method, std::string const& data);
    void set_host (std::string const& host);
    void set_path (std::string const& path);
    void set_port (unsigned int port);
    void set_agent (std::string const& path);
    void add_header (std::string const& header);
    void set_proxy (std::string const& address, unsigned int port);

    HttpDocPtr request (void);
};

/* ---------------------------------------------------------------- */

inline void
Http::set_data (HttpMethod method, std::string const& data)
{
  this->method = method;
  this->data = data;
}

inline void
Http::set_host (std::string const& host)
{
  this->host = host;
}

inline void
Http::set_path (std::string const& path)
{
  this->path = path;
}

inline void
Http::set_port (unsigned int port)
{
  this->port = port;
}

inline void
Http::set_agent (std::string const& agent)
{
  this->agent = agent;
}

inline void
Http::add_header (std::string const& header)
{
  this->headers.push_back(header);
}

inline void
Http::set_proxy (std::string const& address, unsigned int port)
{
  this->proxy = address;
  this->proxy_port = port;
}

#endif /* HTTP_HEADER */
