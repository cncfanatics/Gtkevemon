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

#ifndef SERVER_LIST_HEADER
#define SERVER_LIST_HEADER

#include <string>
#include <vector>
#include <stdint.h>

#include "server.h"

class ServerList
{
  public:
    static std::vector<Server*> list;

  public:
    static void init_from_config (void);
    static void unload (void);

    static void add_server (std::string const& name,
        std::string const& host, uint16_t port = 26000);
    static void refresh (void);
};

#endif /* SERVER_LIST_HEADER */
