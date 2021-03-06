#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "helpers.h"
#include "evetime.h"
#include "exception.h"
#include "apibase.h"

void
ApiBase::check_node (xmlNodePtr node)
{
  if (!this->locally_cached
      && !xmlStrcmp(node->name, (xmlChar const*)"currentTime"))
  {
    std::string text = this->get_node_text(node);
    EveTime::init_from_eveapi_string(text);
  }

  if (!xmlStrcmp(node->name, (xmlChar const*)"cachedUntil"))
  {
    this->cached_until = this->get_node_text(node);
    this->cached_until_t = EveTime::get_time_for_string(this->cached_until);
  }

  if (!xmlStrcmp(node->name, (xmlChar const*)"error"))
  {
    std::string error;
    std::string text;

    try
    { error = this->get_property(node, "code"); }
    catch (...)
    { error = "<unknown>"; }

    try
    { text = this->get_node_text(node); }
    catch (...)
    { text = "<unknown>"; }

    throw Exception("Error document received. Code: " + error + "\n" + text);
  }
}

/* ---------------------------------------------------------------- */

void
ApiBase::enforce_cache_time (time_t min_cache_time)
{
  time_t evenow = EveTime::get_eve_time();
  time_t min_cached = evenow + min_cache_time;
  if (min_cached > this->cached_until_t)
  {
    this->cached_until_t = min_cached;
    this->cached_until = EveTime::get_gm_time_string(min_cached, false);
  }
}
