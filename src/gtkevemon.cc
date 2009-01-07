#include <csignal>
#include <iostream>
#include <gtkmm/main.h>

#include "argumentsettings.h"
#include "versionchecker.h"
#include "imagestore.h"
#include "serverlist.h"
#include "evetime.h"
#include "config.h"
#include "server.h"
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

  ArgumentSettings::init(argc, argv);

  Config::init_defaults();
  Config::init_user_config();

  ImageStore::init();

  VersionChecker::check_data_files();

  std::signal(SIGINT, signal_received);
  std::signal(SIGTERM, signal_received);

  ServerList::init_from_config();
  EveTime::init_from_config();

  {
    MainGui gui;
    kit.run();
  }

  EveTime::store_to_config();
  ServerList::unload();
  ImageStore::unload();

  Config::unload();

  return 0;
}
