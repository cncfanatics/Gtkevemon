#include <cmath>
#include <iostream>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "exception.h"
#include "helpers.h"
#include "xml.h"
#include "eveapi.h"
#include "apibase.h"
#include "apiskilltree.h"
#include "apicharsheet.h"

ApiCharSheet::ApiCharSheet (EveApiAuth const& auth)
{
  this->auth = auth;
}

/* ---------------------------------------------------------------- */

ApiCharSheetPtr
ApiCharSheet::create (EveApiAuth const& auth)
{
  ApiCharSheetPtr obj(new ApiCharSheet(auth));
  obj->refresh();
  return obj;
}

/* ---------------------------------------------------------------- */

void
ApiCharSheet::refresh (void)
{
  this->implant_int = 0;
  this->implant_mem = 0;
  this->implant_cha = 0;
  this->implant_per = 0;
  this->implant_wil = 0;

  HttpDataPtr doc = EveApi::request_charsheet(this->auth);
  this->parse_xml(doc);

  /* Find bonus attributes for skills. */

  /* Analytical Mind (int): 3377
   * Logic (int): 12376
   *
   * Spatial Awareness (per): 3379
   * Clarity (per): 12387
   *
   * Empathy (cha): 3376
   * Presence (cha): 12383
   *
   * Instant Recall (mem): 3378
   * Eidetic Memory (mem): 12385
   *
   * Iron Will (wil): 3375
   * Focus (wil): 12386
   *
   * Learning (2% to all): 3374
   */
  int learning = this->get_level_for_skill(3374);
  double factor = (double)learning * 0.02f;

  this->skill_int = this->get_level_for_skill(3377);
  this->skill_int += this->get_level_for_skill(12376);
  this->skill_int += (this->base_int
      + this->skill_int + this->implant_int) * factor;

  this->skill_per = this->get_level_for_skill(3379);
  this->skill_per += this->get_level_for_skill(12387);
  this->skill_per += (this->base_per
      + this->skill_per + this->implant_per) * factor;

  this->skill_cha = this->get_level_for_skill(3376);
  this->skill_cha += this->get_level_for_skill(12383);
  this->skill_cha += (this->base_cha
      + this->skill_cha + this->implant_cha) * factor;

  this->skill_mem = this->get_level_for_skill(3378);
  this->skill_mem += this->get_level_for_skill(12385);
  this->skill_mem += (this->base_mem
      + this->skill_mem + this->implant_mem) * factor;

  this->skill_wil = this->get_level_for_skill(3375);
  this->skill_wil += this->get_level_for_skill(12386);
  this->skill_wil += (this->base_wil
      + this->skill_wil + this->implant_wil) * factor;

  this->total_int = this->base_int + this->implant_int + this->skill_int;
  this->total_mem = this->base_mem + this->implant_mem + this->skill_mem;
  this->total_cha = this->base_cha + this->implant_cha + this->skill_cha;
  this->total_per = this->base_per + this->implant_per + this->skill_per;
  this->total_wil = this->base_wil + this->implant_wil + this->skill_wil;

  /* Calculate start SP, destination SP and completed. */
  ApiSkillTreePtr tree = ApiSkillTree::request();
  for (unsigned int i = 0; i < this->skills.size(); ++i)
  {
    ApiSkill const& skill = tree->get_skill_from_id(skills[i].id);

    if (skills[i].level == 0)
      skills[i].points_start = 0;
    else
      skills[i].points_start = (int)::ceil(250.0 * skill.rank
          * ::pow(32.0, (skills[i].level - 1) / 2.0));

    skills[i].points_dest = (int)::ceil(250.0 * skill.rank
        * ::pow(32.0, (skills[i].level) / 2.0));

    skills[i].completed = (double)(skills[i].points - skills[i].points_start)
        / (double)(skills[i].points_dest - skills[i].points_start);
  }
}

/* ---------------------------------------------------------------- */

void
ApiCharSheet::parse_xml (HttpDataPtr doc)
{
  this->skills.clear();

  std::cout << "Parsing XML: CharacterSheet.xml ..." << std::endl;
  XmlDocumentPtr xml = XmlDocument::create(doc->data, doc->size);
  xmlNodePtr root = xml->get_root_element();
  this->parse_recursive(root);
}

/* ---------------------------------------------------------------- */

void
ApiCharSheet::parse_recursive (xmlNodePtr node)
{
  while (node != 0)
  {
    bool recurse_node = true;
    if (node->type == XML_ELEMENT_NODE)
    {
      /* Let the base class know of some fields. */
      this->check_node(node);

      if (!xmlStrcmp(node->name, (xmlChar const*)"row"))
      {
        /* Prepare a new skill to append. */
        ApiCharSheetSkill skill;

        /* Fetch these fields. If they are not there, it's an error. */
        try
        {
          skill.id = Helpers::get_int_from_string
              (this->get_property(node, "typeID"));
          skill.points = Helpers::get_int_from_string
              (this->get_property(node, "skillpoints"));
        }
        catch (Exception& e)
        {
          throw Exception(e + " in element \"row\"");
        }

        /* Fetch the level field. If it's not there, ignore the skill.
         * This might happen if the skill in "unpublished" (removed?). */
        try
        {
          skill.level = Helpers::get_int_from_string
              (this->get_property(node, "level"));

          /* Add skill to list. */
          this->skills.push_back(skill);
        }
        catch (Exception& e)
        {
          std::cout << "Warning: Ignoring skill without "
              "\"level\" attribute: " << skill.id << std::endl;
        }
      }

      this->find_implant_bonus(node, "memoryBonus", this->implant_mem);
      this->find_implant_bonus(node, "willpowerBonus", this->implant_wil);
      this->find_implant_bonus(node, "perceptionBonus", this->implant_per);
      this->find_implant_bonus(node, "intelligenceBonus", this->implant_int);
      this->find_implant_bonus(node, "charismaBonus", this->implant_cha);

      this->set_string_if_node_text(node, "name", this->name);
      this->set_string_if_node_text(node, "race", this->race);
      this->set_string_if_node_text(node, "bloodLine", this->bloodline);
      this->set_string_if_node_text(node, "gender", this->gender);
      this->set_string_if_node_text(node, "corporationName", this->corp);
      this->set_string_if_node_text(node, "balance", this->balance);

      this->set_int_if_node_text(node, "intelligence", this->base_int);
      this->set_int_if_node_text(node, "memory", this->base_mem);
      this->set_int_if_node_text(node, "charisma", this->base_cha);
      this->set_int_if_node_text(node, "perception", this->base_per);
      this->set_int_if_node_text(node, "willpower", this->base_wil);
    }

    if (recurse_node)
      this->parse_recursive(node->children);

    node = node->next;
  }
}

/* ---------------------------------------------------------------- */

void
ApiCharSheet::find_implant_bonus (xmlNodePtr node, char const* name, int& var)
{
  if (!xmlStrcmp(node->name, (xmlChar const*)name))
  {
    node = node->children;
    while (node != 0)
    {
      this->set_int_if_node_text(node, "augmentatorValue", var);
      node = node->next;
    }
  }
}

/* ---------------------------------------------------------------- */

int
ApiCharSheet::get_level_for_skill (int id)
{
  ApiCharSheetSkill* skill = this->get_skill_for_id(id);

  if (skill != 0)
    return skill->level;

  /* Return level 0 if skill is not in the list. */
  return 0;
}

/* ---------------------------------------------------------------- */

ApiCharSheetSkill*
ApiCharSheet::get_skill_for_id (int id)
{
  for (unsigned int i = 0; i < this->skills.size(); ++i)
  {
    if (this->skills[i].id == id)
      return &this->skills[i];
  }

  return 0;
}
