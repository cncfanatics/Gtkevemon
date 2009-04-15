#ifndef WIN32
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
#else
# define WIN32_LEAN_AND_MEAN
# include <ws2tcpip.h>
#endif
#include <cstring>
#include <cstdlib>
#include <cerrno>

#include "exception.h"
#include "netdnslookup.h"

NET_NAMESPACE_BEGIN

in_addr_t
DNSLookup::get_hostname (char const* dnsname)
{
  struct addrinfo hints;
  struct addrinfo *res;
  ::memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = 0;
  hints.ai_flags = AI_ADDRCONFIG;
  int retval = ::getaddrinfo(dnsname, 0, &hints, &res);
  if (retval != 0)
  {
    throw Exception("getaddrinfo() failed: "
        + std::string(::gai_strerror(retval)));
  }

  in_addr_t result = ((struct sockaddr_in*)res->ai_addr)->sin_addr.s_addr;
  ::freeaddrinfo(res);

  return result;
}

/* ---------------------------------------------------------------- */

in_addr_t
DNSLookup::get_hostname (std::string const& dnsname)
{
  return DNSLookup::get_hostname(dnsname.c_str());
}

NET_NAMESPACE_END
