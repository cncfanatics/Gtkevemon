#include <iostream>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "xml.h"
#include "exception.h"
#include "apicharlist.h"

void
ApiCharacterList::parse_xml (void)
{
  this->chars.clear();

  std::cout << "Parsing XML: Characters.xml ..." << std::endl;
  XmlDocumentPtr xml = XmlDocument::create
      (&this->http_data->data[0], this->http_data->data.size());
  xmlNodePtr root = xml->get_root_element();
  this->parse_eveapi_tag(root);
}

/* ---------------------------------------------------------------- */

void
ApiCharacterList::parse_eveapi_tag (xmlNodePtr node)
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
ApiCharacterList::parse_result_tag (xmlNodePtr node)
{
  for (; node != 0; node = node->next)
  {
    if (node->type != XML_ELEMENT_NODE)
      continue;

    if (!xmlStrcmp(node->name, (xmlChar const*)"rowset"))
    {
      std::string name = this->get_property(node, "name");
      if (name == "characters")
        this->parse_characters(node->children);
    }
  }
}

/* ---------------------------------------------------------------- */

void
ApiCharacterList::parse_characters (xmlNodePtr node)
{
  for (; node != 0; node = node->next)
  {
    if (node->type != XML_ELEMENT_NODE)
      continue;

    if (!xmlStrcmp(node->name, (xmlChar const*)"row"))
    {
      try
      {
        ApiCharListEntry entry;
        entry.name = this->get_property(node, "name");
        entry.char_id = this->get_property(node, "characterID");
        entry.corp = this->get_property(node, "corporationName");
        entry.corp_id = this->get_property(node, "corporationID");
        this->chars.push_back(entry);
      }
      catch (Exception& e)
      {
        throw Exception(e + " in element \"row\"");
      }
    }
  }
}
