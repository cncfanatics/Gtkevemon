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
  Gtk::Main kit(&argc, &argv);

  ::signal(SIGINT, signal_received);
  ::signal(SIGTERM, signal_received);

  ArgumentSettings::init(argc, argv);

  Config::init_defaults();
  Config::init_user_config();

  ServerList::init_from_config();
  EveTime::init_from_config();
  ImageStore::init();

  {
    MainGui gui;
    kit.run();
  }

  ImageStore::unload();
  EveTime::store_to_config();
  ServerList::unload();

  Config::unload();

  return 0;
}
