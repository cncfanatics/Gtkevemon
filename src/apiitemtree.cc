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
}

/* ---------------------------------------------------------------- */

void
ApiItemTree::parse_xml (std::string const& doc)
{
  /* TODO: Clear members. */
  this->version = 0;

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
      || xmlStrcmp(node->name, (xmlChar const*)"ItemDB")) // TODO
    throw Exception("Invalid tag. expecting <ItemDB> node");

  try
  { this->version = this->get_property_int(node, "dataVersion"); }
  catch (...)
  { }

  node = node->children;

  /* Look for the version tags. */
  while (node != 0)
  {
    /* TODO */

    node = node->next;
  }
}
