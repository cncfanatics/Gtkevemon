#include <iostream>

#include "config.h"
#include "exception.h"
#include "thread.h"
#include "serverlist.h"

/* Static members. */
std::vector<Server> ServerList::list;

/* ---------------------------------------------------------------- */

class ServerChecker : public Thread
{
  protected:
    void* run (void);
};

/* ---------------------------------------------------------------- */

void*
ServerChecker::run (void)
{
  for (unsigned int i = 0; i < ServerList::list.size(); ++i)
  {
    try
    {
      if (!ServerList::list[i].is_refreshing())
        ServerList::list[i].refresh();
    }
    catch (Exception& s)
    {
      std::cout << "Error getting server status: " << s << std::endl;
    }
  }

  delete this;
  return 0;
}

/* ================================================================ */

void
ServerList::init_from_config (void)
{
  ConfSectionPtr servers = Config::conf.get_section("servermonitor");
  for (conf_values_t::iterator iter = servers->values_begin();
      iter != servers->values_end(); iter++)
  {
    ServerList::add_server(iter->first.substr(2), **iter->second);
  }

  ConfValuePtr check = Config::conf.get_value("settings.startup_servercheck");
  if (check->get_bool())
    ServerList::refresh();
}

/* ---------------------------------------------------------------- */

void
ServerList::store_to_config (void)
{
}

/* ---------------------------------------------------------------- */

void
ServerList::add_server (std::string const& name,
    std::string const& host, uint16_t port)
{
  ServerList::list.push_back(Server(name, host, port));
}

/* ---------------------------------------------------------------- */

void
ServerList::refresh (void)
{
  /* Prevents the creation of a thread if not neccessary. */
  if (ServerList::list.size() == 0)
    return;

  std::cout << "Refreshing servers..." << std::endl;

  ServerChecker* checker = new ServerChecker;
  checker->create();
}
