#ifndef EVE_TIME_HEADER
#define EVE_TIME_HEADER

#include <string>

#define EVE_TIME_FORMAT "%Y-%m-%d %H:%M:%S"

class EveTime
{
  private:
    /* Difference between the eve time and the local time. */
    static time_t timediff;
    static bool initialized;

  public:
    /* Calculate the time difference from a string. */
    static void init_from_eveapi_string (std::string const& evetime);
    /* Retrieve the time difference from config file. */
    static void init_from_config (void);
    /* Stores the time difference to config file. */
    static void store_to_config (void);

    /* Returns the current eve time as string. */
    static std::string get_eve_time_string (void);
    /* Returns the current local time as string. */
    static std::string get_local_time_string (void);
    /* Returns a formatted time string from a given time,
     * either using local time or GMT. */
    static std::string get_local_time_string (time_t time);
    static std::string get_gm_time_string (time_t time);

    /* Adjusts the given local time to be in EVE time. */
    static time_t adjust_local_time (time_t time);
    /* Adjusts the given EVE time to be in local time. */
    static time_t adjust_eve_time (time_t time);

    /* Returns EVE simple time. */
    static time_t get_eve_time (void);

    /* Returns a local simple time for a given time string. */
    static time_t get_time_for_string (std::string const& timestr);

    /* Returns if the EVE time has been initialized. Otherwise
     * the EVE time is equal to the local time. */
    static bool is_initialized (void);
};

#endif /* EVE_TIME_HEADER */
