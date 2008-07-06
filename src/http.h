#ifndef HTTP_HEADER
#define HTTP_HEADER

#include <iostream>
#include <vector>
#include <string>
#include "ref_ptr.h"

enum HttpMethod
{
  HTTP_METHOD_GET,
  HTTP_METHOD_POST
};

/* ---------------------------------------------------------------- */

class HttpData;
typedef ref_ptr<HttpData> HttpDataPtr;

class HttpData
{
  protected:
    HttpData (void);
    HttpData (size_t size);

  public:
    std::vector<std::string> headers;
    char* data;
    size_t size;

  public:
    static HttpDataPtr create (void);
    static HttpDataPtr create (size_t size);
    ~HttpData (void);

    void alloc (size_t size);
};

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
    uint16_t port;
    std::vector<std::string> headers;

    std::string proxy;
    uint16_t proxy_port;

    unsigned int get_int_from_hex (std::string const& str);
    size_t data_readline (std::string& dest,
        std::vector<char> const& data, size_t pos);

  public:
    Http (void);
    Http (std::string const& host, std::string const& path);

    void set_host (std::string const& host);
    void set_path (std::string const& path);
    void set_port (uint16_t port);
    void set_agent (std::string const& path);
    void set_data (HttpMethod method, std::string const& data);
    void add_header (std::string const& header);
    void set_proxy (std::string const& address, uint16_t port);

    HttpDataPtr request (void);
};

/* ---------------------------------------------------------------- */

inline
HttpData::HttpData (void)
{
  this->data = 0;
  this->size = 0;
}

inline
HttpData::HttpData (size_t size)
{
  this->data = 0;
  this->alloc(size);
}

inline HttpDataPtr
HttpData::create (void)
{
  return HttpDataPtr(new HttpData);
}

inline HttpDataPtr
HttpData::create (size_t size)
{
  return HttpDataPtr(new HttpData(size));
}

inline
HttpData::~HttpData (void)
{
  delete [] this->data;
}

inline void
HttpData::alloc (size_t size)
{
  delete [] this->data;
  this->data = new char[size];
  this->size = size;
}

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
Http::set_port (uint16_t port)
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
Http::set_proxy (std::string const& address, uint16_t port)
{
  this->proxy = address;
  this->proxy_port = port;
}

#endif /* HTTP_HEADER */
