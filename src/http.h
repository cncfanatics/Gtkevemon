/*
 * This file is part of GtkEveMon.
 *
 * GtkEveMon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with GtkEveMon. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HTTP_HEADER
#define HTTP_HEADER

#include <vector>
#include <string>
#include <stdint.h>

#include "ref_ptr.h"
#include "nettcpsocket.h"
#include "httpstatus.h"

enum HttpMethod
{
  HTTP_METHOD_GET,
  HTTP_METHOD_POST
};

/* ---------------------------------------------------------------- */

enum HttpState
{
  HTTP_STATE_READY,
  HTTP_STATE_CONNECTING,
  HTTP_STATE_REQUESTING,
  HTTP_STATE_RECEIVING,
  HTTP_STATE_DONE,
  HTTP_STATE_ERROR
};

/* ---------------------------------------------------------------- */

class HttpData;
typedef ref_ptr<HttpData> HttpDataPtr;

class HttpData
{
  protected:
    HttpData (void);

  public:
    HttpStatusCode http_code;
    std::vector<std::string> headers;
    std::vector<char> data;

  public:
    static HttpDataPtr create (void);

    /* This is for debugging purposes. */
    void dump_headers (void);
    void dump_data (void);
};

/* ---------------------------------------------------------------- */

/* Class for very simple requesting of documents over HTTP. */
class Http
{
  private:
    /* User specified connection information. */
    HttpMethod method;
    std::string data;
    std::string host;
    std::string path;
    std::string agent;
    uint16_t port;
    std::vector<std::string> headers;
    std::string proxy;
    uint16_t proxy_port;

    /* Tracking the HTTP state. */
    HttpState http_state;
    size_t bytes_read;
    size_t bytes_total;

  private:
    void initialize_defaults (void);

    /* Stages of the HTTP request. */
    Net::TCPSocket initialize_connection (void);
    void send_http_headers (Net::TCPSocket& sock);
    HttpDataPtr read_http_reply (Net::TCPSocket& sock);

    /* Helpers. */
    unsigned int get_uint_from_hex (std::string const& str);
    unsigned int get_uint_from_str (std::string const& str);
    //ssize_t socket_read_line (int sock, std::string& line);
    std::size_t http_data_read (Net::TCPSocket& sock, char* buf,
        std::size_t size);
    HttpStatusCode get_http_status_code (std::string const& header);

  public:
    Http (void);
    Http (std::string const& url);
    Http (std::string const& host, std::string const& path);

    /* Set host and path with a single URL. */
    void set_url (std::string const& url);
    /* Set host and path separately. */
    void set_host (std::string const& host);
    void set_path (std::string const& path);
    /* Set port, defaults to 80. */
    void set_port (uint16_t port);
    /* Set user agent, defaults to VerySimpleHttpRequester. */
    void set_agent (std::string const& path);
    /* Set either GET or POST data. */
    void set_data (HttpMethod method, std::string const& data);
    /* Adds a user defined header, e.g.
     * Authorization: Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ== */
    void add_header (std::string const& header);
    /* Set HTTP proxy server. */
    void set_proxy (std::string const& address, uint16_t port);

    /* Information about the progress. */
    size_t get_bytes_read (void) const;
    /* Information about the total size. This may be zero! */
    size_t get_bytes_total (void) const;

    /* Request the document. This will block until transfer is completed. */
    HttpDataPtr request (void);
};

/* ---------------------------------------------------------------- */

inline
HttpData::HttpData (void)
{
}

inline HttpDataPtr
HttpData::create (void)
{
  return HttpDataPtr(new HttpData);
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

inline size_t
Http::get_bytes_read (void) const
{
  return this->bytes_read;
}

inline size_t
Http::get_bytes_total (void) const
{
  return this->bytes_total;
}

#endif /* HTTP_HEADER */
