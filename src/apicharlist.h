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

#ifndef API_CHAR_LIST_HEADER
#define API_CHAR_LIST_HEADER

#include <string>
#include <vector>
#include <libxml/parser.h>

#include "ref_ptr.h"
#include "http.h"
#include "apibase.h"

class ApiCharListEntry;
typedef std::vector<ApiCharListEntry> ApiCharList;

class ApiCharListEntry
{
  public:
    std::string name;
    std::string char_id;
    std::string corp;
    std::string corp_id;
};

/* ---------------------------------------------------------------- */

class ApiCharacterList;
typedef ref_ptr<ApiCharacterList> ApiCharacterListPtr;

class ApiCharacterList : public ApiBase
{
  /* Some internal stuff. */
  protected:
    ApiCharacterList (void);
    void parse_xml (void);
    void parse_eveapi_tag (xmlNodePtr node);
    void parse_result_tag (xmlNodePtr node);
    void parse_characters (xmlNodePtr node);

  /* Publicly available collection of gathered data. */
  public:
    ApiCharList chars;

  public:
    static ApiCharacterListPtr create (void);
    void set_api_data (EveApiData const& data);
};

/* ---------------------------------------------------------------- */

inline
ApiCharacterList::ApiCharacterList (void)
{
}

inline ApiCharacterListPtr
ApiCharacterList::create (void)
{
  return ApiCharacterListPtr(new ApiCharacterList);
}

inline void
ApiCharacterList::set_api_data (EveApiData const& data)
{
  this->ApiBase::set_api_data(data);
  this->parse_xml();
}

#endif /* API_CHAR_LIST_HEADER */
