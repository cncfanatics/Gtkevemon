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
  private:
    EveApiAuth auth;
    ApiCharList chars;

  protected:
    ApiCharacterList (EveApiAuth const& auth);
    void parse_xml (HttpDataPtr doc);
    void parse_recursive (xmlNodePtr node);

  public:
    static ApiCharacterListPtr create (EveApiAuth const& auth);
    void refresh (void);

    ApiCharList& get_list (void);
};

/* ---------------------------------------------------------------- */

inline ApiCharList&
ApiCharacterList::get_list (void)
{
  return this->chars;
}

#endif /* API_CHAR_LIST_HEADER */
