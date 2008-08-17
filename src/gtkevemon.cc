#include <csignal>
#include <iostream>
#include <gtkmm/main.h>

#include "argumentsettings.h"
#include "evetime.h"
#include "config.h"
#include "serverlist.h"
#include "server.h"
#include "imagestore.h"
#include "maingui.h"

void
signal_received (int signum)
{
  signum = 0;
  Gtk::Main::quit();
}

/* ---------------------------------------------------------------- */

int
main (int argc, char* argv[])
{
  Config::init_defaults();
  Config::init_user_config();

  ::signal(SIGINT, signal_received);
  ::signal(SIGTERM, signal_received);

  ServerList::init_from_config();
  EveTime::init_from_config();
  Gtk::Main kit(&argc, &argv);
  ArgumentSettings::init(argc, argv);
  ImageStore::init();

  {
    MainGui gui;
    kit.run();
  }

  ImageStore::unload();
  EveTime::store_to_config();
  ServerList::store_to_config();

  Config::unload();

  return 0;
}
