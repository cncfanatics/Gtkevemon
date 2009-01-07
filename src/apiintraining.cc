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
  this->ApiBase::set_api_data(data);
  this->parse_xml();

  /* Do some checking if end time is expired. */
  if (this->in_training && this->end_time_t < EveTime::get_eve_time())
    this->in_training = false;

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
  this->parse_recursive(root);
}

/* ---------------------------------------------------------------- */

void
ApiInTraining::parse_recursive (xmlNodePtr node)
{
  xmlNode *cur_node = node;
  while (cur_node != 0)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      /* Let the base class know of some fields. */
      this->check_node(cur_node);

      if (!xmlStrcmp(cur_node->name, (xmlChar const*)"trainingEndTime"))
      {
        this->end_time = this->get_node_text(cur_node);
        this->end_time_t = EveTime::get_time_for_string(this->end_time);
      }

      if (!xmlStrcmp(cur_node->name, (xmlChar const*)"trainingStartTime"))
      {
        this->start_time = this->get_node_text(cur_node);
        this->start_time_t = EveTime::get_time_for_string(this->start_time);
      }

      this->set_int_if_node_text(cur_node,
          "trainingTypeID", this->skill);
      this->set_int_if_node_text(cur_node,
          "trainingStartSP", this->start_sp);
      this->set_int_if_node_text(cur_node,
          "trainingDestinationSP", this->dest_sp);
      this->set_int_if_node_text(cur_node,
          "trainingToLevel", this->to_level);
      this->set_bool_if_node_text(cur_node,
          "skillInTraining", this->in_training);
    }

    this->parse_recursive(cur_node->children);
    cur_node = cur_node->next;
  }
}
