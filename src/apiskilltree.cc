#include <fstream>
#include <iostream>
#include <cstring>
#include <cerrno>

#include "config.h"
#include "exception.h"
#include "xml.h"
#include "helpers.h"
#include "skilltree.xml.h"
#include "apiskilltree.h"

ApiSkillTreePtr ApiSkillTree::instance;

/* ---------------------------------------------------------------- */

ApiSkillTreePtr
ApiSkillTree::request (void)
{
  if (ApiSkillTree::instance.get() == 0)
  {
    ApiSkillTree::instance = ApiSkillTreePtr(new ApiSkillTree);
    ApiSkillTree::instance->refresh();
  }

  return ApiSkillTree::instance;
}

/* ---------------------------------------------------------------- */

ApiSkillTree::ApiSkillTree (void)
{
}

/* ---------------------------------------------------------------- */

void
ApiSkillTree::refresh (void)
{
  /* Try a series of possible file names. */
  std::vector<std::string> filenames;
  filenames.push_back("../xml/SkillTree.xml");
  filenames.push_back("SkillTree.xml");
  filenames.push_back(Config::get_conf_dir() + "/SkillTree.xml");

  std::cout << "Searching XML: SkillTree.xml ...";
  std::cout.flush();

  std::string content;
  for (unsigned int i = 0; i < filenames.size(); ++i)
  {
    std::string filename(filenames[i]);
    std::ifstream in(filename.c_str());
    if (in.fail())
      continue;

    std::cout << " Using local file." << std::endl;

    std::string line;
    while (std::getline(in, line))
      content += line + "\n";
    in.close();
    break;
  }

  /* If the SkillTree.xml file could not been found,
   * use the build-in version. */
  if (content.empty())
  {
    std::cout << " Using build-in string." << std::endl;
    content = skilltree_string;
  }

  //HttpDataPtr data = HttpData::create(content.size() + 1);
  //::memcpy(data->data, &content[0], content.size() * 1);

  this->parse_xml(content);
}

/* ---------------------------------------------------------------- */

void
ApiSkillTree::parse_xml (std::string const& doc)
{
  this->skills.clear();
  this->groups.clear();

  std::cout << "Parsing XML: SkillTree.xml ..." << std::endl;

  XmlDocumentPtr xml = XmlDocument::create(doc);
  xmlNodePtr root = xml->get_root_element();
  this->parse_eveapi_tag(root);
}

/* ---------------------------------------------------------------- */

void
ApiSkillTree::parse_eveapi_tag (xmlNodePtr node)
{
  if (node->type != XML_ELEMENT_NODE
      || xmlStrcmp(node->name, (xmlChar const*)"eveapi"))
  {
    std::cout << "Invalid tag. expecting <eveapi> node" << std::endl;
    return;
  }

  node = node->children;

  /* Look for the result tag. */
  while (node != 0)
  {
    if (node->type == XML_ELEMENT_NODE
        && !xmlStrcmp(node->name, (xmlChar const*)"result"))
    {
      //std::cout << "Found <result> tag" << std::endl;
      this->parse_result_tag(node->children);
    }
    node = node->next;
  }
}

/* ---------------------------------------------------------------- */

void
ApiSkillTree::parse_result_tag (xmlNodePtr node)
{
  /* Look for the rowset tag. It's for the skill group rowset. */
  while (node != 0)
  {
    if (node->type == XML_ELEMENT_NODE
        && !xmlStrcmp(node->name, (xmlChar const*)"rowset"))
    {
      //std::cout << "Found <rowset> tag for skillgroups" << std::endl;
      this->parse_groups_rowset(node->children);
    }
    node = node->next;
  }
}

/* ---------------------------------------------------------------- */

void
ApiSkillTree::parse_groups_rowset (xmlNodePtr node)
{
  /* Look for row tags. These are for the skill groups. */
  while (node != 0)
  {
    if (node->type == XML_ELEMENT_NODE
        && !xmlStrcmp(node->name, (xmlChar const*)"row"))
    {
      ApiSkillGroup group;
      group.name = this->get_property(node, "groupName");
      std::string group_id_str = this->get_property(node, "groupID");
      group.id = Helpers::get_int_from_string(group_id_str);

      //std::cout << "Inserting group: " << group.name << std::endl;
      this->groups.insert(std::make_pair(group.id, group));

      this->parse_groups_row(node->children);
    }
    node = node->next;
  }
}

/* ---------------------------------------------------------------- */

void
ApiSkillTree::parse_groups_row (xmlNodePtr node)
{
  /* Look for the rowset tag. It's forthe skills rowset. */
  while (node != 0)
  {
    if (node->type == XML_ELEMENT_NODE
        && !xmlStrcmp(node->name, (xmlChar const*)"rowset"))
    {
      //std::cout << "Found <rowset> tag for skills" << std::endl;
      this->parse_skills_rowset(node->children);
    }
    node = node->next;
  }
}

/* ---------------------------------------------------------------- */

void
ApiSkillTree::parse_skills_rowset (xmlNodePtr node)
{
  /* Look for row tags. These are for the skills. */
  while (node != 0)
  {
    if (node->type == XML_ELEMENT_NODE
        && !xmlStrcmp(node->name, (xmlChar const*)"row"))
    {
      ApiSkill skill;
      skill.name = this->get_property(node, "typeName");
      std::string group_id_str = this->get_property(node, "groupID");
      std::string skill_id_str = this->get_property(node, "typeID");
      skill.id = Helpers::get_int_from_string(skill_id_str);
      skill.group = Helpers::get_int_from_string(group_id_str);

      this->parse_skills_row(skill, node->children);

      //std::cout << "Inserting skill:   " << skill.name << std::endl;
      this->skills.insert(std::make_pair(skill.id, skill));

    }
    node = node->next;
  }
}

/* ---------------------------------------------------------------- */

void
ApiSkillTree::parse_skills_row (ApiSkill& skill, xmlNodePtr node)
{
  while (node != 0)
  {
    if (node->type == XML_ELEMENT_NODE)
    {
      this->set_string_if_node_text(node, "description", skill.desc);
      this->set_int_if_node_text(node, "rank", skill.rank);

      if (!xmlStrcmp(node->name, (xmlChar const*)"rowset")
          && this->get_property(node, "name") == "requiredSkills")
        this->parse_skill_requirements(skill, node->children);

      if (!xmlStrcmp(node->name, (xmlChar const*)"requiredAttributes"))
        this->parse_skill_attribs(skill, node->children);
    }
    node = node->next;
  }
}

/* ---------------------------------------------------------------- */

void
ApiSkillTree::parse_skill_requirements (ApiSkill& skill, xmlNodePtr node)
{
  while (node != 0)
  {
    if (node->type == XML_ELEMENT_NODE)
    {
      if (!xmlStrcmp(node->name, (xmlChar const*)"row"))
      {
        std::string type_id = this->get_property(node, "typeID");
        std::string level = this->get_property(node, "skillLevel");
        skill.deps.push_back(std::make_pair
            (Helpers::get_int_from_string(type_id),
            Helpers::get_int_from_string(level)));
      }
    }
    node = node->next;
  }
}

/* ---------------------------------------------------------------- */

void
ApiSkillTree::parse_skill_attribs (ApiSkill& skill, xmlNodePtr node)
{
  std::string primary;
  std::string secondary;

  while (node != 0)
  {
    if (node->type == XML_ELEMENT_NODE)
    {
      this->set_string_if_node_text(node, "primaryAttribute", primary);
      this->set_string_if_node_text(node, "secondaryAttribute", secondary);
    }
    node = node->next;
  }

  if (primary.empty() || secondary.empty())
  {
    std::cout << "Error determining attributes" << std::endl;
    return;
  }

  this->set_attribute(skill.primary, primary);
  this->set_attribute(skill.secondary, secondary);
}

/* ---------------------------------------------------------------- */

void
ApiSkillTree::set_attribute (ApiAttrib& var, std::string const& str)
{
  if (str == "memory")
    var = API_ATTRIB_MEMORY;
  else if (str == "charisma")
    var = API_ATTRIB_CHARISMA;
  else if (str == "intelligence")
    var = API_ATTRIB_INTELLIGENCE;
  else if (str == "perception")
    var = API_ATTRIB_PERCEPTION;
  else if (str == "willpower")
    var = API_ATTRIB_WILLPOWER;
  else
    std::cout << "Error finding attribute for " << str << std::endl;
}

/* ---------------------------------------------------------------- */

ApiSkill const&
ApiSkillTree::get_skill_from_id (int id) const
{
  ApiSkillMap::const_iterator iter = this->skills.find(id);
  if (iter == this->skills.end())
    throw Exception("Cannot find skill ID: "
        + Helpers::get_string_from_int(id));

  return iter->second;
}

/* ---------------------------------------------------------------- */

ApiSkillGroup const&
ApiSkillTree::get_group_from_id (int id) const
{
  ApiSkillGroupMap::const_iterator iter = this->groups.find(id);
  if (iter == this->groups.end())
    throw Exception("Cannot find skill group ID: "
        + Helpers::get_string_from_int(id));

  return iter->second;
}

/* ---------------------------------------------------------------- */

char const*
ApiSkillTree::get_attrib_name (ApiAttrib const& attrib)
{
  switch (attrib)
  {
    case API_ATTRIB_INTELLIGENCE:
      return "Intelligence";
    case API_ATTRIB_MEMORY:
      return "Memory";
    case API_ATTRIB_CHARISMA:
      return "Charisma";
    case API_ATTRIB_PERCEPTION:
      return "Perception";
    case API_ATTRIB_WILLPOWER:
      return "Willpower";
    default:
      break;
  }

  return "Unknown";
}

/* ---------------------------------------------------------------- */

void
ApiSkill::debug (void) const
{
  std::cout << "Skill: " << this->name << " (" << this->id << ")" << std::endl
      << "  Rank: " << this->rank << ", Group: " << this->group << std::endl
      << "  Attribs: " << this->primary << " / " << this->secondary << std::endl
      << "  Deps: ";
  for (unsigned int i = 0; i < this->deps.size(); ++i)
    std::cout << this->deps[i].first << " level " << this->deps[i].second << ", ";
  std::cout << std::endl;
}
