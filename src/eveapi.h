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

#include "asynchttp.h"

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

/* ---------------------------------------------------------------- */

/*
 * Possible documents that can be requested from the EVE API.
 */
enum EveApiDocType
{
  EVE_API_DOCTYPE_CHARLIST,
  EVE_API_DOCTYPE_CHARSHEET,
  EVE_API_DOCTYPE_INTRAINING
};

/* ---------------------------------------------------------------- */

class EveApiData : public AsyncHttpData
{
  public:
    bool locally_cached;

    EveApiData (void);
    EveApiData (AsyncHttpData const& data);
};

/* ---------------------------------------------------------------- */

/*
 * Class that fetches EVE API documents synchronously or
 * asynchronously with a simplified and comfortable interface.
 */
class EveApiFetcher
{
  private:
    bool busy;
    EveApiAuth auth;
    EveApiDocType type;
    sigc::signal<void, EveApiData> sig_done;
    sigc::connection conn_sigdone;

  protected:
    AsyncHttp* setup_fetcher (void);
    void async_reply (AsyncHttpData data);
    void process_caching (EveApiData& data);

  public:
    EveApiFetcher (void);
    EveApiFetcher (EveApiAuth const& auth, EveApiDocType type);
    ~EveApiFetcher (void);

    void set_auth (EveApiAuth const& auth);
    void set_doctype (EveApiDocType type);

    void request (void);
    void async_request (void);

    sigc::signal<void, EveApiData>& signal_done (void);
    bool is_busy (void);

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

inline
EveApiData::EveApiData (void)
{
  this->locally_cached = false;
}

inline
EveApiData::EveApiData (AsyncHttpData const& data)
{
  this->data = data.data;
  this->exception = data.exception;
  this->locally_cached = false;
}

inline
EveApiFetcher::EveApiFetcher (void) : busy(false)
{
}

inline
EveApiFetcher::EveApiFetcher (EveApiAuth const& auth, EveApiDocType type)
  : busy(false), auth(auth), type(type)
{
}

inline void
EveApiFetcher::set_auth (EveApiAuth const& auth)
{
  this->auth = auth;
}

inline void
EveApiFetcher::set_doctype (EveApiDocType type)
{
  this->type = type;
}

inline sigc::signal<void, EveApiData>&
EveApiFetcher::signal_done (void)
{
  return this->sig_done;
}

inline bool
EveApiFetcher::is_busy (void)
{
  return this->busy;
}

#endif /* EVE_API_HEADER */
