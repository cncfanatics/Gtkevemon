#ifndef CONFIG_HEADER
#define CONFIG_HEADER

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
    static void unload (void);

    static std::string const& get_conf_dir (void);
    static std::string const& get_filename (void);
};

/* ---------------------------------------------------------------- */

inline void
Config::unload (void)
{
  Config::conf.to_file(Config::filename);
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
