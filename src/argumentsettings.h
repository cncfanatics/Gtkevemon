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

#ifndef ARGUMENT_SETTINGS_HEADER
#define ARGUMENT_SETTINGS_HEADER

#include <string>

class ArgumentSettings
{
  private:
    static void show_help (void);

  public:
    static int argc;
    static char** argv;

    static bool start_minimized;
    static std::string config_dir;

  public:
    static void init (int argc, char** argv);
};

#endif /* ARGUMENT_SETTINGS_HEADER */
