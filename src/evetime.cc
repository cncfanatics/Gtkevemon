#include <iostream>
#include <cstring>
#include <ctime>

#include "evetime.h"
#include "config.h"

time_t EveTime::timediff = 0;
bool EveTime::initialized = false;

/* ---------------------------------------------------------------- */

void
EveTime::init_from_eveapi_string (std::string const& evetime)
{
  time_t eve = EveTime::get_time_for_string(evetime);
  time_t local = EveTime::get_local_time();

  EveTime::timediff = eve - local;
  EveTime::initialized = true;

  //std::cout << "EVE time has been set. Difference: "
  //    << EveTime::timediff << std::endl;
}

/* ---------------------------------------------------------------- */

void
EveTime::init_from_config (void)
{
  if (Config::conf.get_value("evetime.valid")->get_bool())
  {
    EveTime::timediff = (time_t)Config::conf.get_value
        ("evetime.difference")->get_int();
    EveTime::initialized = true;

    //std::cout << "EVE time has been set from config. Difference: "
    //    << EveTime::timediff << std::endl;
  }
}

/* ---------------------------------------------------------------- */

void
EveTime::store_to_config (void)
{
  Config::conf.get_value("evetime.valid")->set(EveTime::initialized);
  Config::conf.get_value("evetime.difference")->set((int)EveTime::timediff);
}

/* ---------------------------------------------------------------- */

std::string
EveTime::get_eve_time_string (void)
{
  time_t eve_time = EveTime::get_eve_time();
  return EveTime::get_gm_time_string(eve_time);
}

/* ---------------------------------------------------------------- */

std::string
EveTime::get_local_time_string (void)
{
  time_t local = EveTime::get_local_time();
  return EveTime::get_local_time_string(local);
}

/* ---------------------------------------------------------------- */

std::string
EveTime::get_local_time_string (time_t time)
{
  char buffer[20];
  strftime(buffer, 20, EVE_TIME_FORMAT, ::localtime(&time));
  return std::string(buffer);
}

/* ---------------------------------------------------------------- */

std::string
EveTime::get_gm_time_string (time_t time)
{
  char buffer[20];
  strftime(buffer, 20, EVE_TIME_FORMAT, ::gmtime(&time));
  return std::string(buffer);
}

/* ---------------------------------------------------------------- */

time_t
EveTime::adjust_local_time (time_t time)
{
  return time - EveTime::timediff;
}

/* ---------------------------------------------------------------- */

time_t
EveTime::adjust_eve_time (time_t time)
{
  return time + EveTime::timediff;
}

/* ---------------------------------------------------------------- */

time_t
EveTime::get_eve_time (void)
{
  return EveTime::get_local_time() + EveTime::timediff;
}

/* ---------------------------------------------------------------- */

time_t
EveTime::get_local_time (void)
{
  return ::time(0);
}

/* ---------------------------------------------------------------- */

time_t
EveTime::get_time_for_string (std::string const& timestr)
{
  struct tm tm;
  ::memset(&tm, '\0', sizeof(struct tm));
  char* tmp = ::strptime(timestr.c_str(), EVE_TIME_FORMAT, &tm);
  if (tmp == 0)
  {
    std::cout << "Warning: Unable to parse time string!" << std::endl;
    return (time_t)0;
  }
  time_t ret = ::timegm(&tm);
  return ret;
}

/* ---------------------------------------------------------------- */

bool
EveTime::is_initialized (void)
{
  return EveTime::initialized;
}
