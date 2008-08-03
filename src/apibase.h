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

#include "http.h"

class ApiBase
{
  protected:
    HttpDataPtr http_data;
    std::string cached_until;
    time_t cached_until_t;

    /* Extracts some common information like errors,
     * the EVE time and the cache time. */
    void check_node (xmlNodePtr node);

    /* Some helper functions. */
    std::string get_property (xmlNodePtr node, char const* name);
    std::string get_node_text (xmlNodePtr node);

    void set_string_if_node_text (xmlNodePtr node, char const* node_name,
        std::string& target);
    void set_int_if_node_text (xmlNodePtr node, char const* node_name,
        int& target);
    void set_bool_if_node_text (xmlNodePtr node, char const* node_name,
        bool& target);

  public:
    std::string const& get_cached_until (void) const;
    time_t get_cached_until_t (void) const;
    HttpDataPtr get_http_data (void) const;
};

/* ---------------------------------------------------------------- */

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

#endif /* API_BASE_HEADER */
