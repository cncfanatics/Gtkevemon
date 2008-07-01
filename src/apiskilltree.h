#ifndef API_SKILL_TREE_HEADER
#define API_SKILL_TREE_HEADER

#include <string>
#include <map>

#include "http.h"
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

class ApiSkill
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
    void parse_xml (HttpDocPtr doc);
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
    static ApiSkillTreePtr request (void);
    void refresh (void);

    ApiSkillMap skills;
    ApiSkillGroupMap groups;

    ApiSkill const& get_skill_from_id (int id) const;
    ApiSkillGroup const& get_group_from_id (int id) const;

    char const* get_attrib_name (ApiAttrib const& attrib);
};

#endif /* API_SKILL_TREE_HEADER */
