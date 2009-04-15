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

#ifndef NET_SOCKET_HEADER
#define NET_SOCKET_HEADER

#include <string>
#include "netdefines.h"

NET_NAMESPACE_BEGIN

class Socket
{
  protected:
    NetSocketFD sock;

  public:
    /**
     * This constructor creates an unconnected SocketBase object.
     * It sets "sock" to -1 and "conntected" to false.
     */
    Socket (void);

    /**
     * This constructor creates an connected SocketBase object
     * from an active file descriptor. This must not neccesarily
     * be a socket and can be every type of file descriptor.
     */
    Socket (NetSocketFD sock_fd);

    /**
     * The virtual destructor does nothing.
     */
    virtual ~Socket (void);

    /**
     * The `close' function closes the connection. If there is still data
     * waiting to be transmitted over the connection, normally `close'
     * tries to complete this transmission.
     */
    virtual void close (void);

    /**
     * The `shutdown_input' function closes the reception channel.
     */
    virtual void shutdown_input (void);

    /**
     * The `shutdown_output' function closes the transmission channel.
     */
    virtual void shutdown_output (void);

    /**
     * The `is_connected' function returns true iff this
     * socket is connected to a host.
     */
    virtual bool is_connected (void) const;

    /**
     * The `input_available` functions returns true iff input
     * is available on this file descriptor.
     */
    virtual bool input_available (void) const;

    /**
     * The `read' function reads up to "size" bytes, storing the results
     * in "buffer". This is not necessarily a character string, and no
     * terminating null character is added.
     *
     * The return value is the number of bytes actually read.  This
     * might be less than "size" if there aren't that many bytes left or
     * if there aren't that many bytes immediately available. A value of
     * zero indicates end-of-file (except if the value of the "size"
     * argument is also zero).
     */
    std::size_t read (void* buffer, std::size_t size, std::size_t offset = 0);

    /**
     * The `full_read' function reads up to "size" bytes, storing the
     * results in "buffer". This is not necessarily a character string,
     * and no terminating null character is added.
     *
     * The only difference to the `read' function above is, that it does
     * not return if no bytes are immediately available.
     */
    std::size_t full_read (void* buffer, std::size_t size,
        std::size_t offset = 0);

    /**
     * The `read_line' function reads characters up to the line breaking
     * character '\n'. None of the line breaking characters such as '\r'
     * or '\n' are removed. The string may not contain any line breaking
     * characters if there are no more characters left, or "max_len" has
     * been reached. "max_len == 0" is treated as nonfinit.
     *
     * This function returns zero-sized strings if end-of-file has been
     * reached.
     */
    void read_line (std::string& result, std::size_t max_len = 0);

    /**
     * The `write' function writes up to "size" bytes from "buffer".  The
     * data in "buffer" is not necessarily a character string and a null
     * character is output like any other character.
     *
     * The return value is the number of bytes actually written.  This
     * may be "size", but can always be smaller.  Your program should
     * always call `write' in a loop, iterating until all the data is
     * written, or call `full_write'.
     */
    std::size_t write (void const* buffer, std::size_t size,
        std::size_t offset = 0);

    /**
     * The `full_write' function writes up to "size" bytes from "buffer".
     * The data in "buffer" is not necessarily a character string and a
     * null character is output like any other character.
     *
     * The return value is the number of bytes actually written.  This
     * may be "size", but can be smaller if no more bytes can be written.
     */
    std::size_t full_write (void const* buffer, std::size_t size,
        std::size_t offset = 0);

    /**
     * The `write_str' function writes the string "text" without any line
     * breaking or null terminating characters.
     */
    void write_str (std::string const& text);

    /**
     * The `write_line' function writes the string "text" and appends a
     * newline if "text" does not already end with a newline.
     */
    void write_line (std::string const& text);

    /**
     * The `get_socket_fd' function returns the bare file descriptor.
     */
    NetSocketFD get_socket_fd (void);
};

NET_NAMESPACE_END

#endif /* NET_SOCKET_HEADER */
