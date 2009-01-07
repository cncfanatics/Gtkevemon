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
  this->parse_recursive(root);
}

/* ---------------------------------------------------------------- */

void
ApiCharacterList::parse_recursive (xmlNodePtr node)
{
  xmlNode *cur_node = node;
  while (cur_node != 0)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      /* Let the base class know of some fields. */
      this->check_node(cur_node);

      //std::cout << "Element name: " << cur_node->name << std::endl;
      if (!xmlStrcmp(cur_node->name, (xmlChar const*)"row"))
      {
        try
        {
          ApiCharListEntry entry;
          entry.name = this->get_property(cur_node, "name");
          entry.char_id = this->get_property(cur_node, "characterID");
          entry.corp = this->get_property(cur_node, "corporationName");
          entry.corp_id = this->get_property(cur_node, "corporationID");
          this->chars.push_back(entry);
        }
        catch (Exception& e)
        {
          throw Exception(e + " in element \"row\"");
        }
      }
    }

    this->parse_recursive(cur_node->children);
    cur_node = cur_node->next;
  }
}

