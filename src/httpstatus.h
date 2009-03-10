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

#ifndef HTTP_STATUS_HEADER
#define HTTP_STATUS_HEADER

/*
 * HTTP status codes received from:
 * http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
 */

#define HTTP_CODE_100_STR "Continue"
#define HTTP_CODE_101_STR "Switching Protocols"
#define HTTP_CODE_200_STR "OK"
#define HTTP_CODE_201_STR "Created"
#define HTTP_CODE_202_STR "Accepted"
#define HTTP_CODE_203_STR "Non-Authoritative Information"
#define HTTP_CODE_204_STR "No Content"
#define HTTP_CODE_205_STR "Reset Content"
#define HTTP_CODE_206_STR "Partial Content"
#define HTTP_CODE_300_STR "Multiple Choices"
#define HTTP_CODE_301_STR "Moved Permanently"
#define HTTP_CODE_302_STR "Found"
#define HTTP_CODE_303_STR "See Other"
#define HTTP_CODE_304_STR "Not Modified"
#define HTTP_CODE_305_STR "Use Proxy"
#define HTTP_CODE_306_STR "(Unused)"
#define HTTP_CODE_307_STR "Temporary Redirect"
#define HTTP_CODE_400_STR "Bad Request"
#define HTTP_CODE_401_STR "Unauthorized"
#define HTTP_CODE_402_STR "Payment Required"
#define HTTP_CODE_403_STR "Forbidden"
#define HTTP_CODE_404_STR "Not Found"
#define HTTP_CODE_405_STR "Method Not Allowed"
#define HTTP_CODE_406_STR "Not Acceptable"
#define HTTP_CODE_407_STR "Proxy Authentication Required"
#define HTTP_CODE_408_STR "Request Timeout"
#define HTTP_CODE_409_STR "Conflict"
#define HTTP_CODE_410_STR "Gone"
#define HTTP_CODE_411_STR "Length Required"
#define HTTP_CODE_412_STR "Precondition Failed"
#define HTTP_CODE_413_STR "Request Entity Too Large"
#define HTTP_CODE_414_STR "Request-URI Too Long"
#define HTTP_CODE_415_STR "Unsupported Media Type"
#define HTTP_CODE_416_STR "Requested Range Not Satisfiable"
#define HTTP_CODE_417_STR "Expectation Failed"
#define HTTP_CODE_500_STR "Internal Server Error"
#define HTTP_CODE_501_STR "Not Implemented"
#define HTTP_CODE_502_STR "Bad Gateway"
#define HTTP_CODE_503_STR "Service Unavailable"
#define HTTP_CODE_504_STR "Gateway Timeout"
#define HTTP_CODE_505_STR "HTTP Version Not Supported"
#define HTTP_CODE_STR_UNKNOWN "Unknown HTTP Code"

typedef unsigned short HttpStatusCode;

class HttpStatus
{
  public:
    static char const* get_string (HttpStatusCode code);
};

/* ---------------------------------------------------------------- */

inline char const*
HttpStatus::get_string (HttpStatusCode code)
{
  switch (code)
  {
    case 100: return HTTP_CODE_100_STR;
    case 101: return HTTP_CODE_101_STR;
    case 200: return HTTP_CODE_200_STR;
    case 201: return HTTP_CODE_201_STR;
    case 202: return HTTP_CODE_202_STR;
    case 203: return HTTP_CODE_203_STR;
    case 204: return HTTP_CODE_204_STR;
    case 205: return HTTP_CODE_205_STR;
    case 206: return HTTP_CODE_206_STR;
    case 300: return HTTP_CODE_300_STR;
    case 301: return HTTP_CODE_301_STR;
    case 302: return HTTP_CODE_302_STR;
    case 303: return HTTP_CODE_303_STR;
    case 304: return HTTP_CODE_304_STR;
    case 305: return HTTP_CODE_305_STR;
    case 306: return HTTP_CODE_306_STR;
    case 307: return HTTP_CODE_307_STR;
    case 400: return HTTP_CODE_400_STR;
    case 401: return HTTP_CODE_401_STR;
    case 402: return HTTP_CODE_402_STR;
    case 403: return HTTP_CODE_403_STR;
    case 404: return HTTP_CODE_404_STR;
    case 405: return HTTP_CODE_405_STR;
    case 406: return HTTP_CODE_406_STR;
    case 407: return HTTP_CODE_407_STR;
    case 408: return HTTP_CODE_408_STR;
    case 409: return HTTP_CODE_409_STR;
    case 410: return HTTP_CODE_410_STR;
    case 411: return HTTP_CODE_411_STR;
    case 412: return HTTP_CODE_412_STR;
    case 413: return HTTP_CODE_413_STR;
    case 414: return HTTP_CODE_414_STR;
    case 415: return HTTP_CODE_415_STR;
    case 416: return HTTP_CODE_416_STR;
    case 417: return HTTP_CODE_417_STR;
    case 500: return HTTP_CODE_500_STR;
    case 501: return HTTP_CODE_501_STR;
    case 502: return HTTP_CODE_502_STR;
    case 503: return HTTP_CODE_503_STR;
    case 504: return HTTP_CODE_504_STR;
    case 505: return HTTP_CODE_505_STR;
    default: return HTTP_CODE_STR_UNKNOWN;
  }

  return HTTP_CODE_STR_UNKNOWN;
}

#endif /* HTTP_STATUS_HEADER */
