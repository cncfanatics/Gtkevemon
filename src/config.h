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

#ifndef CONFIG_HEADER
#define CONFIG_HEADER

#include <string>

#include "asynchttp.h"
#include "conf.h"

class Config
{
  private:
    static std::string conf_dir;
    static std::string filename;

  public:
    static Conf conf;

  public:
    static void init_defaults (void);
    static void init_user_config (void);
    static void save_to_file (void);
    static void unload (void);

    static std::string const& get_conf_dir (void);
    static std::string const& get_filename (void);

    /* Helper function to setup HTTP requests. */
    static void setup_http (AsyncHttp* fetcher);
};

/* ---------------------------------------------------------------- */

inline void
Config::save_to_file (void)
{
  Config::conf.to_file(Config::filename);
}

inline void
Config::unload (void)
{
  Config::save_to_file();
}

inline std::string const&
Config::get_conf_dir (void)
{
  return Config::conf_dir;
}

inline std::string const&
Config::get_filename (void)
{
  return Config::filename;
}

#endif /* CONFIG_HEADER */
