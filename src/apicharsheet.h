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
#include "apibase.h"
#include "apiskilltree.h"

struct ApiCharSheetSkill
{
  int id;
  int level;
  int points;
  int points_start;
  int points_dest;
  double completed;

  ApiSkill const* details;
};

/* ---------------------------------------------------------------- */

struct ApiCharAttribs
{
  double intl;
  double mem;
  double cha;
  double per;
  double wil;
};

/* ---------------------------------------------------------------- */

class ApiCharSheet;
typedef ref_ptr<ApiCharSheet> ApiCharSheetPtr;

class ApiCharSheet : public ApiBase
{
  /* Some internal stuff. */
  protected:
    ApiCharSheet (void);
    void parse_xml (void);
    void parse_recursive (xmlNodePtr node);
    void find_implant_bonus (xmlNodePtr node, char const* name, double& var);

  /* Publicly available collection of gathered data. */
  public:
    bool valid;

    /* Basic char information. */
    std::string char_id;
    std::string name;
    std::string race;
    std::string bloodline;
    std::string gender;
    std::string corp;
    std::string balance;

    /* Attribute values for the character. */
    ApiCharAttribs base;
    ApiCharAttribs implant;
    ApiCharAttribs skill;
    ApiCharAttribs total;

    /* The vector of all known skills. */
    std::vector<ApiCharSheetSkill> skills;

  public:
    static ApiCharSheetPtr create (void);
    void set_api_data (EveApiData const& data);

    int get_level_for_skill (int id);
    ApiCharSheetSkill* get_skill_for_id (int id);

    static int calc_start_sp (int level, int rank);
    static int calc_dest_sp (int level, int rank);

    double get_sppm_for_skill (ApiSkill const* skill);
    double get_sppm_for_skill (ApiSkill const* skill,
        ApiCharAttribs const& attribs);
};

/* ---------------------------------------------------------------- */

inline
ApiCharSheet::ApiCharSheet (void) : valid(false)
{
}

inline ApiCharSheetPtr
ApiCharSheet::create (void)
{
  ApiCharSheetPtr obj(new ApiCharSheet);
  return obj;
}

inline double
ApiCharSheet::get_sppm_for_skill (ApiSkill const* skill)
{
  return this->get_sppm_for_skill(skill, this->total);
}

#endif /* API_CHAR_SHEET_HEADER */
