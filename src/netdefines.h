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

#ifndef NET_DEFINES_HEADER
#define NET_DEFINES_HEADER

/* Namespace for the networking. */
#define NET_NAMESPACE_BEGIN namespace Net {
#define NET_NAMESPACE_END }

#ifdef WIN32
  typedef ULONG in_addr_t;
  typedef SOCKET NetSocketFD;
# define NET_INVALID_SOCKET INVALID_SOCKET;
#else
  typedef int NetSocketFD;
# define NET_INVALID_SOCKET -1
#endif

#endif /* NET_DEFINES_HEADER */
