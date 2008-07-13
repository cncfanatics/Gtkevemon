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

#ifndef API_CHAR_SHEET_HEADER
#define API_CHAR_SHEET_HEADER

#include <string>
#include <vector>
#include <libxml/parser.h>

#include "ref_ptr.h"
#include "http.h"
#include "eveapi.h"
#include "apibase.h"

class ApiCharSheetSkill
{
  public:
    int id;
    int points;
    int points_start;
    int points_dest;
    int level;

    double completed;
};

/* ---------------------------------------------------------------- */

class ApiCharSheet;
typedef ref_ptr<ApiCharSheet> ApiCharSheetPtr;

class ApiCharSheet : public ApiBase
{
  public:
    EveApiAuth auth;

    std::string name;
    std::string race;
    std::string bloodline;
    std::string gender;
    std::string corp;
    std::string balance;

    /* Base values for the character. */
    int base_int;
    int base_mem;
    int base_cha;
    int base_per;
    int base_wil;

    /* Attribute values from implants. */
    int implant_int;
    int implant_mem;
    int implant_cha;
    int implant_per;
    int implant_wil;

    /* Values from attribute skills (e.g. clarity) and lerning skill. */
    double skill_int;
    double skill_mem;
    double skill_cha;
    double skill_per;
    double skill_wil;

    /* The total values for the attributes. It's the sum of the above. */
    double total_int;
    double total_mem;
    double total_cha;
    double total_per;
    double total_wil;

    /* The vector of all known skills. */
    std::vector<ApiCharSheetSkill> skills;

  protected:
    ApiCharSheet (EveApiAuth const& auth);
    void parse_xml (HttpDataPtr doc);
    void parse_recursive (xmlNodePtr node);
    void find_implant_bonus (xmlNodePtr node, char const* name, int& var);

  public:
    static ApiCharSheetPtr create (EveApiAuth const& auth);
    void refresh (void);
    int get_level_for_skill (int id);
    ApiCharSheetSkill* get_skill_for_id (int id);
};

#endif /* API_CHAR_SHEET_HEADER */
