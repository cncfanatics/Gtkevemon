#include <fstream>
#include <iostream>
#include <vector>

#include "xml.h"
#include "config.h"
#include "helpers.h"
#include "exception.h"
#include "certtree.xml.h"
#include "apicerttree.h"

ApiCertTreePtr ApiCertTree::instance;

/* ---------------------------------------------------------------- */

ApiCertTreePtr
ApiCertTree::request (void)
{
  if (ApiCertTree::instance.get() == 0)
  {
    ApiCertTree::instance = ApiCertTreePtr(new ApiCertTree);
    ApiCertTree::instance->refresh();
  }

  return ApiCertTree::instance;
}

/* ---------------------------------------------------------------- */

ApiCertTree::ApiCertTree (void)
{
  /* Default version is zero - means the data is not initialized. */
  this->version = 0;
}

/* ---------------------------------------------------------------- */

void
ApiCertTree::refresh (void)
{
  /* Try a series of possible file names. */
  std::vector<std::string> filenames;
  filenames.push_back("../xml/CertificateTree.xml");
  filenames.push_back("CertificateTree.xml");
  filenames.push_back(Config::get_conf_dir() + "/CertificateTree.xml");

  std::cout << "Seeking XML: CertificateTree.xml ...";
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

  if (content.empty())
  {
    std::cout << " Using build-in string." << std::endl;
    content = certtree_string;
  }

  this->parse_xml(content);
  //this->debug_dump();
}

/* ---------------------------------------------------------------- */

void
ApiCertTree::parse_xml (std::string const& doc)
{
  this->certificates.clear();
  this->categories.clear();
  this->classes.clear();
  this->version = 0;

  std::cout << "Parsing XML: CertificateTree.xml ...";
  std::cout.flush();

  try
  {
    XmlDocumentPtr xml = XmlDocument::create(doc);
    xmlNodePtr root = xml->get_root_element();
    this->parse_eveapi_tag(root);
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
ApiCertTree::parse_eveapi_tag (xmlNodePtr node)
{
  if (node->type != XML_ELEMENT_NODE
      || xmlStrcmp(node->name, (xmlChar const*)"eveapi"))
    throw Exception("Invalid tag. expecting <eveapi> node");

  /* Try to get version information from the file. This will only
   * work if it's a CertificateTree.xml that is prepared for GtkEveMon. */
  try
  { this->version = this->get_property_int(node, "dataVersion"); }
  catch (...)
  { }

  node = node->children;

  /* Look for the result and version tag. */
  while (node != 0)
  {
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
ApiCertTree::parse_result_tag (xmlNodePtr node)
{
  /* Look for the rowset tag. It's for the cert category rowset. */
  while (node != 0)
  {
    if (node->type == XML_ELEMENT_NODE
        && !xmlStrcmp(node->name, (xmlChar const*)"rowset"))
    {
      //std::cout << "Found <rowset> tag for cert categories" << std::endl;
      this->parse_categories_rowset(node->children);
    }
    node = node->next;
  }
}

/* ---------------------------------------------------------------- */

void
ApiCertTree::parse_categories_rowset (xmlNodePtr node)
{
  /* Look for row tags. These are for the cert categories. */
  while (node != 0)
  {
    if (node->type == XML_ELEMENT_NODE
        && !xmlStrcmp(node->name, (xmlChar const*)"row"))
    {
      ApiCertCategory category;
      category.name = this->get_property(node, "categoryName");
      category.id = this->get_property_int(node, "categoryID");

      //std::cout << "Inserting category: " << category.name << std::endl;
      this->categories.insert(std::make_pair(category.id, category));
      this->parse_categories_row(category, node->children);
    }
    node = node->next;
  }
}

/* ---------------------------------------------------------------- */

void
ApiCertTree::parse_categories_row (ApiCertCategory& category, xmlNodePtr node)
{
  /* Look for the rowset tag. It's for the classes rowset. */
  while (node != 0)
  {
    if (node->type == XML_ELEMENT_NODE
        && !xmlStrcmp(node->name, (xmlChar const*)"rowset"))
    {
      //std::cout << "Found <rowset> tag for classes" << std::endl;
      this->parse_classes_rowset(category, node->children);
    }
    node = node->next;
  }
}

/* ---------------------------------------------------------------- */

void
ApiCertTree::parse_classes_rowset (ApiCertCategory& category, xmlNodePtr node)
{
  /* Look for row tags. These are for the cert classes. */
  while (node != 0)
  {
    if (node->type == XML_ELEMENT_NODE
        && !xmlStrcmp(node->name, (xmlChar const*)"row"))
    {
      ApiCertClass certclass;
      certclass.name = this->get_property(node, "className");
      certclass.id = this->get_property_int(node, "classID");
      certclass.cat_id = category.id;

      //std::cout << "Inserting class: " << certclass.name << std::endl;
      this->classes.insert(std::make_pair(certclass.id, certclass));
      this->parse_classes_row(certclass, node->children);
    }
    node = node->next;
  }
}

/* ---------------------------------------------------------------- */

void
ApiCertTree::parse_classes_row (ApiCertClass& cclass, xmlNodePtr node)
{
  /* Look for the rowset tag. It's for the certificates rowset (at last!). */
  while (node != 0)
  {
    if (node->type == XML_ELEMENT_NODE
        && !xmlStrcmp(node->name, (xmlChar const*)"rowset"))
    {
      //std::cout << "Found <rowset> tag for certificates" << std::endl;
      this->parse_certificates_rowset(cclass, node->children);
    }
    node = node->next;
  }
}

/* ---------------------------------------------------------------- */

void
ApiCertTree::parse_certificates_rowset (ApiCertClass& cclass, xmlNodePtr node)
{
  /* Look for row tags. These are for the certificates. */
  while (node != 0)
  {
    if (node->type == XML_ELEMENT_NODE
        && !xmlStrcmp(node->name, (xmlChar const*)"row"))
    {
      ApiCert certificate;
      certificate.class_id = cclass.id;
      certificate.id = this->get_property_int(node, "certificateID");
      certificate.grade = this->get_property_int(node, "grade");
      certificate.desc = this->get_property(node, "description");

      //std::cout << "Inserting certificate: " << certificate.id << std::endl;
      this->certificates.insert(std::make_pair(certificate.id, certificate));
      this->parse_certificate_row(certificate, node->children);
    }
    node = node->next;
  }
}

/* ---------------------------------------------------------------- */

void
ApiCertTree::parse_certificate_row (ApiCert& cert, xmlNodePtr node)
{
  for (; node != 0; node = node->next)
  {
    if (node->type != XML_ELEMENT_NODE
        || xmlStrcmp(node->name, (xmlChar const*)"rowset"))
      continue;

    std::string name = this->get_property(node, "name");

    if (name == "requiredSkills")
    {
      for (xmlNodePtr cnode = node->children; cnode != 0; cnode = cnode->next)
      {
        if (node->type != XML_ELEMENT_NODE
            || xmlStrcmp(node->name, (xmlChar const*)"row"))
          continue;

        int skill_id = this->get_property_int(cnode, "typeID");
        int skill_level = this->get_property_int(cnode, "level");
        cert.skilldeps.push_back(std::make_pair(skill_id, skill_level));
      }
    }
    else if (name == "requiredCertificates")
    {
      for (xmlNodePtr cnode = node->children; cnode != 0; cnode = cnode->next)
      {
        if (cnode->type != XML_ELEMENT_NODE
            || xmlStrcmp(cnode->name, (xmlChar const*)"row"))
          continue;

        int cert_id = this->get_property_int(cnode, "certificateID");
        int cert_grade = this->get_property_int(cnode, "grade");
        cert.certdeps.push_back(std::make_pair(cert_id, cert_grade));
      }
    }
  }
}

/* ---------------------------------------------------------------- */

ApiCert const*
ApiCertTree::get_certificate_for_id (int id) const
{
  ApiCertMap::const_iterator iter = this->certificates.find(id);
  if (iter == this->certificates.end())
    return 0;

  return &iter->second;
}

/* ---------------------------------------------------------------- */

ApiCertCategory const*
ApiCertTree::get_category_for_id (int id) const
{
  ApiCertCategoryMap::const_iterator iter = this->categories.find(id);
  if (iter == this->categories.end())
    return 0;

  return &iter->second;
}

/* ---------------------------------------------------------------- */

ApiCertClass const*
ApiCertTree::get_class_for_id (int id) const
{
  ApiCertClassMap::const_iterator iter = this->classes.find(id);
  if (iter == this->classes.end())
    return 0;

  return &iter->second;
}

/* ---------------------------------------------------------------- */

void
ApiCertTree::debug_dump (void)
{
  std::cout << "DEBUG" << std::endl;
  std::cout << "Categories" << std::endl;
  for (ApiCertCategoryMap::iterator i = this->categories.begin();
      i != this->categories.end(); i++)
    std::cout << i->second.id << " / " << i->second.name << std::endl;
  std::cout << std::endl;

  std::cout << "Classes" << std::endl;
  for (ApiCertClassMap::iterator i = this->classes.begin();
      i != this->classes.end(); i++)
    std::cout << i->second.id << " / " << i->second.name << std::endl;
  std::cout << std::endl;

  std::cout << "Certificates" << std::endl;
  for (ApiCertMap::iterator i = this->certificates.begin();
      i != this->certificates.end(); i++)
    std::cout << i->second.id << " / " << i->second.grade << std::endl;
  std::cout << std::endl;
}
