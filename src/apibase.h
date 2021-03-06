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

#ifndef API_BASE_HEADER
#define API_BASE_HEADER

#include <string>
#include <libxml/parser.h>

#include "eveapi.h"
#include "http.h"
#include "xml.h"

enum ApiElementType
{
  API_ELEM_SKILL,
  API_ELEM_CERT,
  API_ELEM_ITEM
};

class ApiElement
{
  public:
    virtual ~ApiElement (void) {}
    virtual ApiElementType get_type (void) const = 0;
};

/* ---------------------------------------------------------------- */

class ApiBase : public XmlBase
{
  protected:
    HttpDataPtr http_data;
    bool locally_cached;
    std::string cached_until;
    time_t cached_until_t;

    /* Extracts some common information like errors,
     * the EVE time and the cache time. */
    void check_node (xmlNodePtr node);

    /* Sets cached_until and cached_until_t with respect
     * to min_cache_time to ensure a minimum cache time.
     * Does not overwrite greater cache times. */
    void enforce_cache_time (time_t min_cache_time);

  public:
    virtual ~ApiBase (void);
    virtual void set_api_data (EveApiData const& data);

    bool is_locally_cached (void) const;
    std::string const& get_cached_until (void) const;
    time_t get_cached_until_t (void) const;
    HttpDataPtr get_http_data (void) const;
};

/* ---------------------------------------------------------------- */

inline
ApiBase::~ApiBase (void)
{
}

inline void
ApiBase::set_api_data (EveApiData const& data)
{
  this->locally_cached = data.locally_cached;
  this->http_data = data.data;
}

inline std::string const&
ApiBase::get_cached_until (void) const
{
  return this->cached_until;
}

inline time_t
ApiBase::get_cached_until_t (void) const
{
  return this->cached_until_t;
}

inline HttpDataPtr
ApiBase::get_http_data (void) const
{
  return this->http_data;
}

inline bool
ApiBase::is_locally_cached (void) const
{
  return this->locally_cached;
}

#endif /* API_BASE_HEADER */
