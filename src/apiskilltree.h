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

#ifndef API_SKILL_TREE_HEADER
#define API_SKILL_TREE_HEADER

#include <vector>
#include <string>
#include <map>

#include "ref_ptr.h"
#include "apibase.h"

enum ApiAttrib
{
  API_ATTRIB_INTELLIGENCE,
  API_ATTRIB_MEMORY,
  API_ATTRIB_CHARISMA,
  API_ATTRIB_PERCEPTION,
  API_ATTRIB_WILLPOWER
};

/* ---------------------------------------------------------------- */

class ApiSkill : public ApiElement
{
  public:
    int id;
    int group;
    int rank;
    std::string name;
    std::string desc;
    ApiAttrib primary;
    ApiAttrib secondary;

    std::vector<std::pair<int, int> > deps;

  public:
    ~ApiSkill (void) {}
    ApiElementType get_type (void) const;
    void debug (void) const;
};

/* ---------------------------------------------------------------- */

class ApiSkillGroup
{
  public:
    int id;
    std::string name;
};

/* ---------------------------------------------------------------- */

class ApiSkillTree;
typedef ref_ptr<ApiSkillTree> ApiSkillTreePtr;
typedef std::map<int, ApiSkill> ApiSkillMap;
typedef std::map<int, ApiSkillGroup> ApiSkillGroupMap;

class ApiSkillTree : public ApiBase
{
  private:
    static ApiSkillTreePtr instance;

  protected:
    ApiSkillTree (void);
    void parse_xml (std::string const& filename);
    void parse_eveapi_tag (xmlNodePtr node);
    void parse_result_tag (xmlNodePtr node);
    void parse_groups_rowset (xmlNodePtr node);
    void parse_groups_row (xmlNodePtr node);
    void parse_skills_rowset (xmlNodePtr node);
    void parse_skills_row (ApiSkill& skill, xmlNodePtr node);
    void parse_skill_requirements (ApiSkill& skill, xmlNodePtr node);
    void parse_skill_attribs (ApiSkill& skill, xmlNodePtr node);

    void set_attribute (ApiAttrib& var, std::string const& str);

  public:
    ApiSkillMap skills;
    ApiSkillGroupMap groups;
    int version;

  public:
    static ApiSkillTreePtr request (void);
    void refresh (void);

    ApiSkill const* get_skill_for_id (int id) const;
    ApiSkillGroup const* get_group_for_id (int id) const;

    static char const* get_attrib_name (ApiAttrib const& attrib);
    static char const* get_attrib_short_name (ApiAttrib const& attrib);
};

/* ---------------------------------------------------------------- */

inline ApiElementType
ApiSkill::get_type (void) const
{
  return API_ELEM_SKILL;
}

#endif /* API_SKILL_TREE_HEADER */
