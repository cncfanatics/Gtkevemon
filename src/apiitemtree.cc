#include <fstream>
#include <iostream>
#include <vector>

#include "xml.h"
#include "config.h"
#include "helpers.h"
#include "exception.h"
#include "apiitemtree.h"

ApiItemTreePtr ApiItemTree::instance;

/* ---------------------------------------------------------------- */

ApiItemTreePtr
ApiItemTree::request (void)
{
  if (ApiItemTree::instance.get() == 0)
  {
    ApiItemTree::instance = ApiItemTreePtr(new ApiItemTree);
    ApiItemTree::instance->refresh();
  }

  return ApiItemTree::instance;
}

/* ---------------------------------------------------------------- */

ApiItemTree::ApiItemTree (void)
{
  /* Default version is zero - means the data is not initialized. */
  this->version = 0;
}

/* ---------------------------------------------------------------- */

void
ApiItemTree::refresh (void)
{
  /* Try a series of possible file names. */
  std::vector<std::string> filenames;
  filenames.push_back("../xml/ItemTree.xml");
  filenames.push_back("ItemTree.xml");
  filenames.push_back(Config::get_conf_dir() + "/ItemTree.xml");

  std::cout << "Seeking XML: ItemTree.xml ...";
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

  /* If the content is empty, we give up. */
  if (content.empty())
  {
    std::cout << " Not found." << std::endl;
    return;
  }

  this->parse_xml(content);
  //this->debug_dump();
}

/* ---------------------------------------------------------------- */

void
ApiItemTree::parse_xml (std::string const& doc)
{
  /* Clear members. */
  this->version = 0;
  this->cats.clear();

  std::cout << "Parsing XML: ItemTree.xml ...";
  std::cout.flush();

  try
  {
    XmlDocumentPtr xml = XmlDocument::create(doc);
    xmlNodePtr root = xml->get_root_element();
    this->parse_root_tag(root);
  }
  catch (Exception& e)
  {
    std::cout << std::endl << "Error: " << e << std::endl;
  }

  std::cout << " Version " << this->version
      << (this->version == 0 ? " (not set)" : "")
      << "." << std::endl;
}

/* ---------------------------------------------------------------- */

void
ApiItemTree::parse_root_tag (xmlNodePtr node)
{
  if (node->type != XML_ELEMENT_NODE
      || xmlStrcmp(node->name, (xmlChar const*)"ItemDB"))
    throw Exception("Invalid tag. expecting <ItemDB> node");

  try
  { this->version = this->get_property_int(node, "dataVersion"); }
  catch (...)
  { }

  this->parse_item_categories(this->cats, node->children);
}

/* ---------------------------------------------------------------- */

void
ApiItemTree::parse_item_categories (ApiItemCategoryList& cats, xmlNodePtr node)
{
  for (; node != 0; node = node->next)
  {
    if (node->type != XML_ELEMENT_NODE)
      continue;

    if (!xmlStrcmp(node->name, (xmlChar const*)"ItemCategory"))
    {
      ApiItemCategory cat;
      cat.id = this->get_property_int(node, "id");
      cat.name = this->get_property(node, "name");
      cat.iconname = this->get_property(node, "icon");
      this->parse_item_category(cat, node->children);
      cats.insert(std::make_pair(cat.id, cat));
    }
  }
}

/* ---------------------------------------------------------------- */

void
ApiItemTree::parse_item_category (ApiItemCategory& cat, xmlNodePtr node)
{
  for (; node != 0; node = node->next)
  {
    if (node->type != XML_ELEMENT_NODE)
      continue;

    this->set_string_if_node_text(node, "Description", cat.description);
    if (!xmlStrcmp(node->name, (xmlChar const*)"SubCategories"))
      this->parse_item_categories(cat.subcats, node->children);
    if (!xmlStrcmp(node->name, (xmlChar const*)"ItemList"))
      this->parse_item_list(cat, node->children);
  }
}

/* ---------------------------------------------------------------- */

void
ApiItemTree::parse_item_list (ApiItemCategory& cat, xmlNodePtr node)
{
  for (; node != 0; node = node->next)
  {
    if (node->type != XML_ELEMENT_NODE)
      continue;

    if (!xmlStrcmp(node->name, (xmlChar const*)"Item"))
    {
      ApiItem item;
      item.id = this->get_property_int(node, "id");
      item.name = this->get_property(node, "name");
      item.iconname = this->get_property(node, "icon");
      this->parse_item(item, node->children);
      cat.items.insert(std::make_pair(item.id, item));
    }
  }
}

/* ---------------------------------------------------------------- */

void
ApiItemTree::parse_item (ApiItem& item, xmlNodePtr node)
{
  for (; node != 0; node = node->next)
  {
    if (node->type != XML_ELEMENT_NODE) continue;
    this->set_string_if_node_text(node, "Description", item.description);

    if (!xmlStrcmp(node->name, (xmlChar const*)"Properties"))
    {
      for (xmlNodePtr n2 = node->children; n2 != 0; n2 = n2->next)
      {
        if (node->type != XML_ELEMENT_NODE) continue;
        if (!xmlStrcmp(node->name, (xmlChar const*)"Property"))
        {
          std::string pname = this->get_property(n2, "name");
          std::string value = this->get_node_text(n2);
          item.properties.push_back(std::make_pair(pname,
              Helpers::get_float_from_string(value)));
        }
      }
    }

    if (!xmlStrcmp(node->name, (xmlChar const*)"RequiredSkills"))
    {
      for (xmlNodePtr n2 = node->children; n2 != 0; n2 = n2->next)
      {
        if (node->type != XML_ELEMENT_NODE) continue;
        if (!xmlStrcmp(node->name, (xmlChar const*)"Skill"))
        {
          int id = this->get_property_int(n2, "id");
          int level = this->get_property_int(n2, "level");
          item.skilldeps.push_back(std::make_pair(id, level));
        }
      }
    }
  }
}

/* ---------------------------------------------------------------- */

void
ApiItemTree::debug_dump (ApiItemCategoryList* cats, int indent)
{
  if (cats == 0)
    cats = &this->cats;

  for (ApiItemCategoryList::iterator iter = cats->begin();
      iter != cats->end(); iter++)
  {
    for (unsigned int j = 0; (int)j < indent; ++j) std::cout << " ";
    std::cout << "+ " << iter->second.name << " (" << iter->first << ")" << std::endl;
    this->debug_dump(&iter->second.subcats, indent + 2);
    this->debug_dump(&iter->second.items, indent + 2);
  }
}

/* ---------------------------------------------------------------- */

void
ApiItemTree::debug_dump (ApiItemList* items, int indent)
{
  for (ApiItemList::iterator iter = items->begin();
      iter != items->end(); iter++)
  {
      for (unsigned int j = 0; (int)j < indent; ++j) std::cout << " ";
      std::cout << "- " << iter->second.name << " (" << iter->first << ")" << std::endl;
  }
}
