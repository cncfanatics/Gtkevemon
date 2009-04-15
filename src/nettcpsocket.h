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

#ifndef NET_TCP_SOCKET_HEADER
#define NET_TCP_SOCKET_HEADER

#include <string>
#ifndef WIN32
#  include <netinet/in.h>
#else
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#include "netdefines.h"
#include "netsocket.h"

NET_NAMESPACE_BEGIN

class TCPSocket : public Socket
{
  private:
    struct sockaddr_in remote;
    struct sockaddr_in local;
    std::size_t timeout;

  public:
    /**
     * This constructor creates an unconnected TCPSocket object. The results
     * of all member functions but `connect' are undefined until the
     * socket is connected.
     */
    TCPSocket (void);

    /**
     * This constructor creates a connected TCPSocket object. Internally
     * the `connect' function is called with the same arguments. Refer
     * to `connect' for further information.
     */
    TCPSocket (std::string const& host, int port);

    /**
     * This constructor creates a connected TCPSocket object from an already
     * connected socket file descriptor. All members are updated with the
     * appropriate data. This constructor is mainly used by the
     * TCPServerSocket for newly accepted connections.
     */
    TCPSocket (int sock_fd);

    /**
     * The `connect' function initiates a connection to the socket whose
     * address is specified by the "host" and "port" arguments. This
     * socket is typically on another machine, and it must be already set
     * up as a server. The "host" argument is assumed to be in the
     * standard dots-and-numbers format.
     *
     * The `connect' function waits until the server responds to the
     * request before it returns.
     */
    void connect (std::string const& host, int port);

    /**
     * Same as the function above but it uses another host format.
     */
    void connect (in_addr_t host, int port);

    /**
     * The `set_connect_timeout' function allows to define a timeout in
     * milli seconds for the connect function. The connect function will
     * fail if no connection has been established if the timeout expires.
     */
    void set_connect_timeout (std::size_t timeout_ms);

    /**
     * The `get_local_port' function returns the remote port.
     */
    int get_local_port (void) const;

    /**
     * The `get_remote_port' function returns the remote port.
     */
    int get_remote_port (void) const;

    /**
     * The `get_local_address' function returns the local address in the
     * standard numbers-and-dots notation.
     */
    std::string get_local_address (void) const;

    /**
     * The `get_remote_address' function returns the remote address in the
     * standard numbers-and-dots notation.
     */
    std::string get_remote_address (void) const;

    /*
     * The `get_full_local' function returns the local address in the
     * standard numbers-and-dots notation, followed by a colon (":") and
     * the local port.
     */
    std::string get_full_local (void) const;

    /*
     * The `get_full_remote' function returns the remote address in the
     * standard numbers-and-dots notation, followed by a colon (":") and
     * the remote port.
     */
    std::string get_full_remote (void) const;
};

NET_NAMESPACE_END

#endif /* NET_TCP_SOCKET_HEADER */
