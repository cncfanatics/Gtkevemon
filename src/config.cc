#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <iostream>
#include <string>
#include "config.h"

#define CONF_HOME_DIR ".gtkevemon"

/* Default configuration is always loaded. The application
 * can rely on these keys to be there all the time even if
 * the values are not in the configuration file. */
char const* default_config =
    "[accounts]\n"
    "[characters]\n"
    "[evetime]\n"
    "  valid = false\n"
    "  difference = 0\n"
    "[network]\n"
    "  use_proxy = false\n"
    "  proxy_address = \n"
    "  proxy_port = 80\n"
    "[notifications]\n"
    "  show_popup_dialog = true\n"
    "  show_tray_icon = true\n"
    "  show_info_bar = true\n"
    "[servermonitor]\n"
    "[settings]\n"
    "  auto_update_sheets = true\n"
    "  detailed_tray_tooltip = true\n"
    "  eve_cache = /\n"
    "  eve_command = eve\n"
    "  minimize_on_close = false\n"
    "  startup_servercheck = true\n"
    "  tray_usage = never\n"
    "[versionchecker]\n"
    "  enabled = true\n"
    "  last_seen = \n";

/* The initial configuration is loaded once if the configuration
 * file is created for the first time. Thus it initializes the
 * configuration but the values can be deleted from the config. */
char const* initial_config =
    "[servermonitor]\n"
    "  0_Tranquility = 87.237.38.200\n"
    "  1_Serenity = 61.129.54.100\n"
    "  2_Singularity = 87.237.38.50\n";

/* ---------------------------------------------------------------- */

Conf Config::conf;
std::string Config::conf_dir;
std::string Config::filename;

/* ---------------------------------------------------------------- */

void
Config::init_defaults (void)
{
  Config::conf.add_from_string(default_config);
}

/* ---------------------------------------------------------------- */

void
Config::init_user_config (void)
{
  /* The first part here determines the user's config directory.
   * If there is no home directory or any other error occures,
   * the current directory or even worse, /tmp is used. */
  std::string user_conf_dir;
  {
    uid_t user_id = ::geteuid();
    struct passwd* user_info = ::getpwuid(user_id);

    if (user_info == 0 || user_info->pw_dir == 0)
    {
      std::cout << "Warning: Couldn't determine home directry!" << std::endl;
      char buffer[512];
      char* ret = ::getcwd(buffer, 512);
      if (ret != 0)
        user_conf_dir = buffer;
      else
      {
        std::cout << "Warning: Couldn't determine CWD!" << std::endl;
        user_conf_dir = "/tmp";
      }
    }
    else
    {
      user_conf_dir = user_info->pw_dir;
      user_conf_dir += "/";
      user_conf_dir += CONF_HOME_DIR;
    }
  }

  //std::cout << "Resolved config directory to: " << user_conf_dir << std::endl;

  /* Check if the GtkEveMon directory exists in the home directry. */
  int dir_exists = ::access(user_conf_dir.c_str(), F_OK);

  if (dir_exists < 0)
  {
    /* Directory does not exists. Create it. */
    std::cout << "Creating config directory: " << user_conf_dir << std::endl;
    int ret = ::mkdir(user_conf_dir.c_str(), S_IRWXU);
    if (ret < 0)
    {
      std::cout << "Error: Couldn't create the config directory!" << std::endl;
      std::cout << "Error: Falling back to /tmp" << std::endl;
      user_conf_dir = "/tmp";
    }
  }

  /* Set the filename, make it official. */
  Config::conf_dir = user_conf_dir;
  Config::filename = user_conf_dir + "/gtkevemon.conf";

  /* Check if the config file is in-place. If not, dump the current
   * default configuration to file. */
  int conf_exists = ::access(Config::filename.c_str(), F_OK);
  if (conf_exists < 0)
  {
    std::cout << "Creating initial config file: gtkevemon.conf" << std::endl;
    Config::conf.add_from_string(initial_config);
    Config::conf.to_file(Config::filename);
  }
  else
  {
    /* Now read the config file. If it's not there,
     * it will be created when GtkEveMon exits. */
    Config::conf.add_from_file(Config::filename);
  }
}
