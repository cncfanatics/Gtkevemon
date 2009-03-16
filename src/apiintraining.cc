#include <iostream>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "xml.h"
#include "evetime.h"
#include "apiintraining.h"

void
ApiInTraining::set_api_data (EveApiData const& data)
{
  this->valid = false;
  this->holds_completed = false;

  this->ApiBase::set_api_data(data);
  this->parse_xml();

  /* Force the training sheet to have at least 10 minutes cache time. */
  this->enforce_cache_time(API_IN_TRAINING_MIN_CACHE_TIME);

  /* Do some checking if end time is expired. In this case we mark
   * the sheet as not in training but also indicate that it holds
   * a valid, completed skill. */
  if (this->in_training && this->end_time_t < EveTime::get_eve_time())
  {
    this->in_training = false;
    this->holds_completed = true;
  }

  // This is to test notifications! NEVER PUT THIS IN A REVISION
  //this->end_time_t = ::time(0);

  this->valid = true;
}

/* ---------------------------------------------------------------- */

void
ApiInTraining::parse_xml (void)
{
  std::cout << "Parsing XML: SkillInTraining.xml ..." << std::endl;
  XmlDocumentPtr xml = XmlDocument::create
      (&this->http_data->data[0], this->http_data->data.size());
  xmlNodePtr root = xml->get_root_element();
  this->parse_eveapi_tag(root);
}

/* ---------------------------------------------------------------- */

void
ApiInTraining::parse_eveapi_tag (xmlNodePtr node)
{
  if (node->type != XML_ELEMENT_NODE
      || xmlStrcmp(node->name, (xmlChar const*)"eveapi"))
    throw Exception("Invalid XML root. Expecting <eveapi> node.");

  node = node->children;

  for (; node != 0; node = node->next)
  {
    /* Let the base class know of some fields. */
    this->check_node(node);

    if (node->type == XML_ELEMENT_NODE
        && !xmlStrcmp(node->name, (xmlChar const*)"result"))
    {
      //std::cout << "Found <result> tag" << std::endl;
      this->parse_result_tag(node->children);
    }
  }
}

/* ---------------------------------------------------------------- */

void
ApiInTraining::parse_result_tag (xmlNodePtr node)
{
  for (; node != 0; node = node->next)
  {
    if (node->type != XML_ELEMENT_NODE)
      continue;

    if (!xmlStrcmp(node->name, (xmlChar const*)"trainingEndTime"))
    {
      this->end_time = this->get_node_text(node);
      this->end_time_t = EveTime::get_time_for_string(this->end_time);
    }

    if (!xmlStrcmp(node->name, (xmlChar const*)"trainingStartTime"))
    {
      this->start_time = this->get_node_text(node);
      this->start_time_t = EveTime::get_time_for_string(this->start_time);
    }

    this->set_int_if_node_text(node, "trainingTypeID", this->skill);
    this->set_int_if_node_text(node, "trainingStartSP", this->start_sp);
    this->set_int_if_node_text(node, "trainingDestinationSP", this->dest_sp);
    this->set_int_if_node_text(node, "trainingToLevel", this->to_level);
    this->set_bool_if_node_text(node, "skillInTraining", this->in_training);
  }
}

/* ---------------------------------------------------------------- */

#if 0
unsigned int
ApiInTraining::get_current_level_sp (void)
{
  time_t evetime = EveTime::get_eve_time();
  if (evetime > this->end_time_t)
    return this->dest_sp;
  if (evetime < this->start_time_t)
    return this->start_sp;

  double fraction = ((double)evetime - (double)this->start_time_t)
      / ((double)this->end_time_t - (double)this->start_time_t);
  return (unsigned int)(fraction * ((double)this->dest_sp
      - (double)this->start_sp));
}
#endif

/* ---------------------------------------------------------------- */

unsigned int
ApiInTraining::get_current_spph (void)
{
  double total_sp = this->dest_sp - this->start_sp;
  double total_time = this->end_time_t - this->start_time_t;

  return (unsigned int)(total_sp * 3600.0 / total_time);
}
