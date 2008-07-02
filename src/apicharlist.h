#ifndef API_CLASSES_HEADER
#define API_CLASSES_HEADER

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

#endif /* API_CLASSES_HEADER */
