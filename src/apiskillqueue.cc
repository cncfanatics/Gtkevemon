#include <iostream>

#include "xml.h"
#include "helpers.h"
#include "evetime.h"
#include "apiskillqueue.h"

void
ApiSkillQueue::set_api_data (EveApiData const& data)
{
  this->valid = false;

  this->ApiBase::set_api_data(data);
  this->parse_xml();

  /* Force the skill queue to have a minimum cache time. */
  this->enforce_cache_time(API_SKILL_QUEUE_MIN_CACHE_TIME);

  this->valid = true;
}

/* ---------------------------------------------------------------- */

void
ApiSkillQueue::parse_xml (void)
{
  std::cout << "Parsing XML: SkillQueue.xml ..." << std::endl;
  XmlDocumentPtr xml = XmlDocument::create
      (&this->http_data->data[0], this->http_data->data.size());
  xmlNodePtr root = xml->get_root_element();
  this->parse_eveapi_tag(root);
}

/* ---------------------------------------------------------------- */

void
ApiSkillQueue::parse_eveapi_tag (xmlNodePtr node)
{
  if (node->type != XML_ELEMENT_NODE
      || xmlStrcmp(node->name, (xmlChar const*)"eveapi"))
    throw Exception("Invalid XML root. Expecting <eveapi> node.");

  for (node = node->children; node != 0; node = node->next)
  {
    if (node->type != XML_ELEMENT_NODE)
      continue;

    /* Let the base class know of some fields. */
    this->check_node(node);

    if (!xmlStrcmp(node->name, (xmlChar const*)"result"))
    {
      //std::cout << "Found <result> tag" << std::endl;
      this->parse_result_tag(node->children);
    }
  }
}

/* ---------------------------------------------------------------- */

void
ApiSkillQueue::parse_result_tag (xmlNodePtr node)
{
  for (; node != 0; node = node->next)
  {
    if (node->type != XML_ELEMENT_NODE)
      continue;

    if (!xmlStrcmp(node->name, (xmlChar const*)"rowset"))
    {
      std::string name = this->get_property(node, "name");
      if (name == "skillqueue")
        this->parse_queue_rowset(node->children);
    }
  }
}

/* ---------------------------------------------------------------- */

void
ApiSkillQueue::parse_queue_rowset (xmlNodePtr node)
{
  for (; node != 0; node = node->next)
  {
    if (node->type != XML_ELEMENT_NODE)
      continue;

    if (!xmlStrcmp(node->name, (xmlChar const*)"row"))
    {
      std::string pos = this->get_property(node, "queuePosition");
      std::string type_id = this->get_property(node, "typeID");
      std::string level = this->get_property(node, "level");
      std::string startsp = this->get_property(node, "startSP");
      std::string endsp = this->get_property(node, "endSP");

      ApiSkillQueueItem item;
      item.start_time = this->get_property(node, "startTime");
      item.end_time = this->get_property(node, "endTime");
      item.queue_pos = Helpers::get_int_from_string(pos);
      item.skill_id = Helpers::get_int_from_string(type_id);
      item.to_level = Helpers::get_int_from_string(level);
      item.start_sp = Helpers::get_int_from_string(startsp);
      item.end_sp = Helpers::get_int_from_string(endsp);
      item.start_time_t = EveTime::get_time_for_string(item.start_time);
      item.end_time_t = EveTime::get_time_for_string(item.end_time);

      this->queue.push_back(item);
    }
  }
}

/* ---------------------------------------------------------------- */

bool
ApiSkillQueue::in_training (void) const
{
  if (this->queue.empty())
    return false;

  if (this->queue.back().end_time_t < EveTime::get_eve_time())
    return false;

  return true;
}

/* ---------------------------------------------------------------- */

ApiSkillQueueItem const*
ApiSkillQueue::get_training_skill (void) const
{
  if (this->queue.empty())
    return 0;

  time_t eve_time = EveTime::get_eve_time();
  for (std::size_t i = 0; i < this->queue.size(); ++i)
    if (this->queue[i].end_time_t >= eve_time)
      return &this->queue[i];

  return 0;
}

/* ---------------------------------------------------------------- */

unsigned int
ApiSkillQueue::get_spph_for_current (void) const
{
  ApiSkillQueueItem const* item = this->get_training_skill();
  if (item == 0)
    return 0;

  double skill_sp = item->end_sp - item->start_sp;
  double train_time = item->end_time_t - item->start_time_t;
  return (unsigned int)(skill_sp * 3600.0 / train_time);
}

/* ---------------------------------------------------------------- */

void
ApiSkillQueue::debug_dump (void)
{
  std::cout << "=== Skill Queue dump ===" << std::endl;
  for (std::size_t i = 0; i < this->queue.size(); ++i)
  {
    ApiSkillQueueItem const& item = this->queue[i];
    std::cout << "Element " << i << ": Position " << item.queue_pos
        << ", Skill ID " << item.skill_id << " (" << item.to_level << ")"
        << ", from " << item.start_sp << " to " << item.end_sp << " SP"
        << ", from unix time " << item.start_time_t
        << " to " << item.end_time_t << std::endl;
  }
}
