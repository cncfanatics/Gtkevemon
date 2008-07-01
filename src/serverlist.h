#ifndef SERVER_LIST_HEADER
#define SERVER_LIST_HEADER

#include <string>
#include <vector>

#include "server.h"

class ServerList
{
  public:
    static std::vector<Server> list;

  public:
    static void init_from_config (void);
    static void store_to_config (void);
    static void add_server (std::string const& name,
        std::string const& host, int port = 26000);
    static void refresh (void);
};

#endif /* SERVER_LIST_HEADER */
