#include <iostream>
#include <gtkmm/main.h>

#include "evetime.h"
#include "config.h"
#include "serverlist.h"
#include "server.h"
#include "imagestore.h"
#include "maingui.h"

int
main (int argc, char* argv[])
{
  Config::init_defaults();
  Config::init_user_config();

  ServerList::init_from_config();
  EveTime::init_from_config();
  Gtk::Main kit(&argc, &argv);
  ImageStore::init();

  new MainGui;
  kit.run();

  ImageStore::unload();
  EveTime::store_to_config();
  ServerList::store_to_config();

  Config::unload();

  return 0;
}
