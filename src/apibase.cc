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
    std::string error = this->get_property(node, "code");
    std::string text = this->get_node_text(node);
    throw Exception("Error document received. Code: " + error + "\n"
        + text);
  }
}

/* ---------------------------------------------------------------- */

std::string
ApiBase::get_node_text (xmlNodePtr node)
{
  xmlChar* text = xmlNodeGetContent(node);
  if (text == 0)
    throw Exception("Could not retrieve error node text");
  std::string ret((char const*)text);
  xmlFree(text);
  return ret;
}

/* ---------------------------------------------------------------- */

std::string
ApiBase::get_property (xmlNodePtr node, char const* name)
{
  xmlChar* text = xmlGetProp(node, (xmlChar const*)name);
  if (text == 0)
    throw Exception(std::string("Could not find property \"") + name + "\"");
  std::string ret((char const*)text);
  xmlFree(text);
  return ret;
}

/* ---------------------------------------------------------------- */

void
ApiBase::set_string_if_node_text (xmlNodePtr node, char const* node_name,
    std::string& target)
{
  if (!xmlStrcmp(node->name, (xmlChar const*)node_name))
    target = this->get_node_text(node);
}

/* ---------------------------------------------------------------- */

void
ApiBase::set_int_if_node_text (xmlNodePtr node, char const* node_name,
    int& target)
{
  if (!xmlStrcmp(node->name, (xmlChar const*)node_name))
    target = Helpers::get_int_from_string(this->get_node_text(node));
}

/* ---------------------------------------------------------------- */

void
ApiBase::set_bool_if_node_text (xmlNodePtr node, char const* node_name,
    bool& target)
{
  if (!xmlStrcmp(node->name, (xmlChar const*)node_name))
    target = (bool)Helpers::get_int_from_string(this->get_node_text(node));
}
