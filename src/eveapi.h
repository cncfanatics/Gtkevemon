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

#ifndef EVE_API_HEADER
#define EVE_API_HEADER

#include <string>

#include "http.h"

/*
 * A class that contains authentication information for the API.
 */
class EveApiAuth
{
  public:
    std::string user_id;
    std::string api_key;
    std::string char_id;

  public:
    EveApiAuth (void);
    EveApiAuth (std::string const& uid, std::string const& apikey);
    EveApiAuth (std::string const& uid, std::string const& apikey,
        std::string const& cid);
};

/*
 * Returns the XML documents from the EVE API.
 * All these functions may throw if there are networking issues.
 */
class EveApi
{
  private:
    static std::string get_post_data (EveApiAuth const& auth);
    static void set_proxy_server (Http& request);

  public:
    static HttpDataPtr request_charlist (EveApiAuth const& auth);
    static HttpDataPtr request_charsheet (EveApiAuth const& auth);
    static HttpDataPtr request_in_training (EveApiAuth const& auth);
};

/* ---------------------------------------------------------------- */

inline
EveApiAuth::EveApiAuth (void)
{
}

inline
EveApiAuth::EveApiAuth (std::string const& uid, std::string const& apikey)
  : user_id(uid), api_key(apikey)
{
}

inline
EveApiAuth::EveApiAuth (std::string const& uid, std::string const& apikey,
    std::string const& cid)
  : user_id(uid), api_key(apikey), char_id(cid)
{
}

#endif /* EVE_API_HEADER */
