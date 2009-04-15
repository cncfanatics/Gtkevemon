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
  Config::init_config_path();
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
