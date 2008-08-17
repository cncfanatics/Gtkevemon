#include <cstdlib>
#include <string>
#include <iostream>

#include "defines.h"
#include "argumentsettings.h"

bool ArgumentSettings::start_minimized = false;

/* ---------------------------------------------------------------- */

void
ArgumentSettings::show_help (void)
{
  std::cout << "Usage: gtkevemon [ options ]" << std::endl
      << "Options:" << std::endl
      << "    -h, --help               Display this text" << std::endl
      << "    -m, --start-minimized    Start gtkevemon minimized" << std::endl;
}

/* ---------------------------------------------------------------- */

void
ArgumentSettings::init (int argc, char** argv)
{
  for (int optind = 1; optind < argc; ++optind)
  {
    if (argv[optind][0] != '-')
      continue;

    std::string sw = argv[optind];
    if (sw == "-m" || sw == "--start-minimized")
    {
      ArgumentSettings::start_minimized = true;
    }
    else if (sw == "-h" || sw == "--help")
    {
      ArgumentSettings::show_help();
      ::exit(0);
    }
    else if (sw == "-v" || sw == "--version")
    {
      std::cout << "GtkEveMon Version: " GTKEVEMON_VERSION_STR << std::endl;
      ::exit(0);
    }
    else
    {
      std::cout << "Unrecognized option: " << sw << std::endl;
    }
  }
}
