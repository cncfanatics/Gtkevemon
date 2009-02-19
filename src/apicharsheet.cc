#include <cmath>
#include <iostream>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "exception.h"
#include "helpers.h"
#include "xml.h"
#include "apibase.h"
#include "apiskilltree.h"
#include "apicerttree.h"
#include "apicharsheet.h"

void
ApiCharSheet::set_api_data (EveApiData const& data)
{
  this->valid = false;
  this->ApiBase::set_api_data(data);

  /* Reset values. */
  this->implant.intl = 0;
  this->implant.mem = 0;
  this->implant.cha = 0;
  this->implant.per = 0;
  this->implant.wil = 0;

  /* Parse the data. */
  this->parse_xml();

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

  this->skill.intl = this->get_level_for_skill(3377);
  this->skill.intl += this->get_level_for_skill(12376);
  this->skill.intl += (this->base.intl
      + this->skill.intl + this->implant.intl) * factor;

  this->skill.per = this->get_level_for_skill(3379);
  this->skill.per += this->get_level_for_skill(12387);
  this->skill.per += (this->base.per
      + this->skill.per + this->implant.per) * factor;

  this->skill.cha = this->get_level_for_skill(3376);
  this->skill.cha += this->get_level_for_skill(12383);
  this->skill.cha += (this->base.cha
      + this->skill.cha + this->implant.cha) * factor;

  this->skill.mem = this->get_level_for_skill(3378);
  this->skill.mem += this->get_level_for_skill(12385);
  this->skill.mem += (this->base.mem
      + this->skill.mem + this->implant.mem) * factor;

  this->skill.wil = this->get_level_for_skill(3375);
  this->skill.wil += this->get_level_for_skill(12386);
  this->skill.wil += (this->base.wil
      + this->skill.wil + this->implant.wil) * factor;

  this->total.intl = this->base.intl + this->implant.intl + this->skill.intl;
  this->total.mem = this->base.mem + this->implant.mem + this->skill.mem;
  this->total.cha = this->base.cha + this->implant.cha + this->skill.cha;
  this->total.per = this->base.per + this->implant.per + this->skill.per;
  this->total.wil = this->base.wil + this->implant.wil + this->skill.wil;

  /* Calculate start SP, destination SP and completed. */
  ApiSkillTreePtr stree = ApiSkillTree::request();
  for (unsigned int i = 0; i < this->skills.size(); ++i)
  {
    ApiCharSheetSkill& cskill = this->skills[i];
    ApiSkill const* skill = stree->get_skill_for_id(cskill.id);
    if (skill == 0)
    {
      std::cout << "Warning: Ignoring unknown skill (ID " << cskill.id
          << "). Update GtkEveMon or SkillTree.xml." << std::endl;
      this->skills.erase(this->skills.begin() + i);
      i -= 1;
      continue;
    }

    cskill.details = skill;
    cskill.points_start = ApiCharSheet::calc_start_sp
        (skills[i].level, skill->rank);
    cskill.points_dest = ApiCharSheet::calc_dest_sp
        (skills[i].level, skill->rank);
    cskill.completed = (double)(skills[i].points - skills[i].points_start)
        / (double)(skills[i].points_dest - skills[i].points_start);
  }

  /* Update certificate field "details". */
  ApiCertTreePtr ctree = ApiCertTree::request();
  for (unsigned int i = 0; i < this->certs.size(); ++i)
  {
    ApiCharSheetCert& ccert = this->certs[i];
    ApiCert const* cert = ctree->get_certificate_for_id(ccert.id);
    if (cert == 0)
    {
      std::cout << "Warning: Ignoring unknown certificate (ID " << ccert.id
          << "). Update GtkEveMon or CertificateTree.xml." << std::endl;
      this->certs.erase(this->certs.begin() + i);
      i -= 1;
      continue;
    }

    ccert.details = cert;
  }

  //this->debug_dump();
  this->valid = true;
}

/* ---------------------------------------------------------------- */

void
ApiCharSheet::parse_xml (void)
{
  this->skills.clear();

  std::cout << "Parsing XML: CharacterSheet.xml ..." << std::endl;
  XmlDocumentPtr xml = XmlDocument::create
      (&this->http_data->data[0], this->http_data->data.size());
  xmlNodePtr root = xml->get_root_element();
  this->parse_eveapi_tag(root);
}

/* ---------------------------------------------------------------- */

void
ApiCharSheet::parse_eveapi_tag (xmlNodePtr node)
{
  if (node->type != XML_ELEMENT_NODE
      || xmlStrcmp(node->name, (xmlChar const*)"eveapi"))
  throw Exception("Invalid tag. expecting <eveapi> node");

  node = node->children;

  while (node != 0)
  {
    /* Let the base class know of some fields. */
    this->check_node(node);

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
ApiCharSheet::parse_result_tag (xmlNodePtr node)
{
  for (; node != 0; node = node->next)
  {
    if (node->type != XML_ELEMENT_NODE)
      continue;

    this->set_string_if_node_text(node, "characterID", this->char_id);
    this->set_string_if_node_text(node, "name", this->name);
    this->set_string_if_node_text(node, "race", this->race);
    this->set_string_if_node_text(node, "bloodLine", this->bloodline);
    this->set_string_if_node_text(node, "gender", this->gender);
    this->set_string_if_node_text(node, "corporationName", this->corp);
    this->set_string_if_node_text(node, "balance", this->balance);
    this->set_string_if_node_text(node, "cloneName", this->clone_name);
    this->set_string_if_node_text(node, "cloneSkillPoints", this->clone_sp);

    if (!xmlStrcmp(node->name, (xmlChar const*)"attributes"))
      this->parse_attribute_tag(node->children);

    if (!xmlStrcmp(node->name, (xmlChar const*)"attributeEnhancers"))
      this->parse_attrib_enhancers_tag(node->children);

    if (!xmlStrcmp(node->name, (xmlChar const*)"rowset"))
    {
      std::string name = this->get_property(node, "name");
      if (name == "skills")
        this->parse_skills_tag(node->children);
      else if (name == "certificates")
        this->parse_certificates_tag(node->children);
    }
  }
}

/* ---------------------------------------------------------------- */

void
ApiCharSheet::parse_attribute_tag (xmlNodePtr node)
{
  for (; node != 0; node = node->next)
  {
    if (node->type != XML_ELEMENT_NODE)
      continue;

    this->set_double_if_node_text(node, "intelligence", this->base.intl);
    this->set_double_if_node_text(node, "memory", this->base.mem);
    this->set_double_if_node_text(node, "charisma", this->base.cha);
    this->set_double_if_node_text(node, "perception", this->base.per);
    this->set_double_if_node_text(node, "willpower", this->base.wil);
  }
}

/* ---------------------------------------------------------------- */

void
ApiCharSheet::parse_attrib_enhancers_tag (xmlNodePtr node)
{
  for (; node != 0; node = node->next)
  {
    if (node->type != XML_ELEMENT_NODE)
      continue;

    this->find_implant_bonus(node, "memoryBonus", this->implant.mem);
    this->find_implant_bonus(node, "willpowerBonus", this->implant.wil);
    this->find_implant_bonus(node, "perceptionBonus", this->implant.per);
    this->find_implant_bonus(node, "intelligenceBonus", this->implant.intl);
    this->find_implant_bonus(node, "charismaBonus", this->implant.cha);
  }
}

/* ---------------------------------------------------------------- */

void
ApiCharSheet::parse_skills_tag (xmlNodePtr node)
{
  for (; node != 0; node = node->next)
  {
    if (node->type != XML_ELEMENT_NODE)
      continue;

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
  }
}

/* ---------------------------------------------------------------- */

void
ApiCharSheet::parse_certificates_tag (xmlNodePtr node)
{
  for (; node != 0; node = node->next)
  {
    if (node->type != XML_ELEMENT_NODE)
      continue;

    if (!xmlStrcmp(node->name, (xmlChar const*)"row"))
    {
      ApiCharSheetCert cert;

      try
      {
        cert.id = Helpers::get_int_from_string
            (this->get_property(node, "certificateID"));
        this->certs.push_back(cert);
      }
      catch (Exception& e)
      {
        throw Exception(e + " in element \"row\"");
      }
    }
  }
}

/* ---------------------------------------------------------------- */

void
ApiCharSheet::find_implant_bonus (xmlNodePtr node, char const* name, double& v)
{
  if (node->type == XML_ELEMENT_NODE
      && !xmlStrcmp(node->name, (xmlChar const*)name))
  {
    node = node->children;
    while (node != 0)
    {
      this->set_double_if_node_text(node, "augmentatorValue", v);
      node = node->next;
    }
  }
}

/* ---------------------------------------------------------------- */

void
ApiCharSheet::add_char_skill (int skill_id, int level)
{
  /* Bail out if we get an invalid level. */
  if (level < 0 || level > 5)
    return;

  ApiCharSheetSkill* cskill = this->get_skill_for_id(skill_id);

  /* Bail out if the character has the skill and level. */
  if (cskill != 0 && cskill->level >= level)
    return;

  /* Create skill if the character does not have it, update otherwise. */
  if (cskill == 0)
  {
    /* Create new skill. */
    ApiSkillTreePtr tree = ApiSkillTree::request();
    ApiSkill const* skill = tree->get_skill_for_id(skill_id);

    ApiCharSheetSkill new_cskill;
    new_cskill.id = skill->id;
    new_cskill.level = level;
    new_cskill.points = 0;
    new_cskill.points_start = ApiCharSheet::calc_start_sp(level, skill->rank);
    new_cskill.points_dest = ApiCharSheet::calc_dest_sp(level, skill->rank);
    new_cskill.completed = 0.0;
    new_cskill.details = skill;

    this->skills.push_back(new_cskill);
  }
  else
  {
    /* Update existing skill. */
    ApiSkill const* skill = cskill->details;

    cskill->level = level;
    cskill->points = 0;
    cskill->points_start = ApiCharSheet::calc_start_sp(level, skill->rank);
    cskill->points_dest = ApiCharSheet::calc_dest_sp(level, skill->rank);
    cskill->completed = 0.0;
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

/* ---------------------------------------------------------------- */

ApiCharSheetCert*
ApiCharSheet::get_cert_for_id (int id)
{
  for (unsigned int i = 0; i < this->certs.size(); ++i)
  {
    if (this->certs[i].id == id)
      return &this->certs[i];
  }

  return 0;
}

/* ---------------------------------------------------------------- */

int
ApiCharSheet::get_grade_for_class (int class_id)
{
  int grade = 0;
  for (unsigned int i = 0; i < this->certs.size(); ++i)
  {
    ApiCert const* cert = this->certs[i].details;
    ApiCertClass const* cclass = cert->class_details;
    if (cclass->id == class_id && cert->grade > grade)
      grade = this->certs[i].details->grade;
  }

  return grade;
}

/* ---------------------------------------------------------------- */

int
ApiCharSheet::calc_start_sp (int level, int rank)
{
  if (level == 0)
    return 0;

  return ApiCharSheet::calc_dest_sp(level - 1, rank);
}

/* ---------------------------------------------------------------- */

int
ApiCharSheet::calc_dest_sp (int level, int rank)
{
  return (int)::ceil(250.0 * rank * ::pow(32.0, level / 2.0));
}

/* ---------------------------------------------------------------- */

double
ApiCharSheet::get_sppm_for_skill (ApiSkill const* skill,
    ApiCharAttribs const& attribs)
{
  double pri;
  double sec;

  switch (skill->primary)
  {
    case API_ATTRIB_INTELLIGENCE: pri = attribs.intl; break;
    case API_ATTRIB_MEMORY:       pri = attribs.mem; break;
    case API_ATTRIB_CHARISMA:     pri = attribs.cha; break;
    case API_ATTRIB_PERCEPTION:   pri = attribs.per; break;
    case API_ATTRIB_WILLPOWER:    pri = attribs.wil; break;
    default: pri = 0.0;
  }

  switch (skill->secondary)
  {
    case API_ATTRIB_INTELLIGENCE: sec = attribs.intl; break;
    case API_ATTRIB_MEMORY:       sec = attribs.mem; break;
    case API_ATTRIB_CHARISMA:     sec = attribs.cha; break;
    case API_ATTRIB_PERCEPTION:   sec = attribs.per; break;
    case API_ATTRIB_WILLPOWER:    sec = attribs.wil; break;
    default: sec = 0.0;
  }

  return (pri + sec / 2.0);
}

/* ---------------------------------------------------------------- */

void
ApiCharSheet::debug_dump (void)
{
  std::cout << "Character dump for " << this->name << std::endl;
  std::cout << "List of all skills:" << std::endl;

  for (unsigned int i = 0; i < this->skills.size(); ++i)
  {
    std::cout << "  " << this->skills[i].details->name
        << " (" << this->skills[i].id << ")" << std::endl;
  }

  std::cout << "List of all certs:" << std::endl;
  for (unsigned int i = 0; i < this->certs.size(); ++i)
  {
    std::cout << "  " << /*this->certs[i].details->name*/ "Unnamed yet"
        << " (" << this->certs[i].id << ")" << std::endl;
  }
}
