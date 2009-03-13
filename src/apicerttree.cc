#include <fstream>
#include <iostream>
#include <vector>

#include "xml.h"
#include "config.h"
#include "helpers.h"
#include "exception.h"
#include "apicerttree.h"

#define CERTTREE_FN "CertificateTree.xml.gz"

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
  filenames.push_back("../xml/" CERTTREE_FN);
  filenames.push_back(CERTTREE_FN);
  filenames.push_back(Config::get_conf_dir() + "/" CERTTREE_FN);

  for (unsigned int i = 0; i < filenames.size(); ++i)
  {
    try
    {
      this->parse_xml(filenames[i]);
      //this->debug_dump();
      return;
    }
    catch (FileException& e)
    {
      /* Ignore file exception. File is probably just not there. */
    }
    catch (Exception& e)
    {
      /* Parse error occured. Report this. */
      std::cout << std::endl << "XML error: " << e << std::endl;
    }
  }

  std::cout << "Seeking XML: " << CERTTREE_FN
      << " not found. Shutdown!" << std::endl;
  ::exit(0);
}

/* ---------------------------------------------------------------- */

void
ApiCertTree::parse_xml (std::string const& filename)
{
  /* Try to read the document. */
  XmlDocumentPtr xml = XmlDocument::create_from_file(filename);
  xmlNodePtr root = xml->get_root_element();

  std::cout << "Parsing XML: " CERTTREE_FN " ...";
  std::cout.flush();

  /* Document was parsed. Reset information. */
  this->certificates.clear();
  this->categories.clear();
  this->classes.clear();
  this->version = 0;

  this->parse_eveapi_tag(root);

  std::cout << " Version " << this->version
      << (this->version == 0 ? " (not set)" : "")
      << ", " << this->certificates.size() << " certs." << std::endl;
}

/* ---------------------------------------------------------------- */

void
ApiCertTree::parse_eveapi_tag (xmlNodePtr node)
{
  if (node->type != XML_ELEMENT_NODE
      || xmlStrcmp(node->name, (xmlChar const*)"eveapi"))
    throw Exception("Invalid tag. Expecting <eveapi> node");

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
      ApiCertCategoryMap::iterator ins = this->categories.insert
          (std::make_pair(category.id, category)).first;
      this->parse_categories_row(&ins->second, node->children);
    }
    node = node->next;
  }
}

/* ---------------------------------------------------------------- */

void
ApiCertTree::parse_categories_row (ApiCertCategory* category, xmlNodePtr node)
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
ApiCertTree::parse_classes_rowset (ApiCertCategory* category, xmlNodePtr node)
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
      certclass.cat_details = category;

      //std::cout << "Inserting class: " << certclass.name << std::endl;
      ApiCertClassMap::iterator ins = this->classes.insert
          (std::make_pair(certclass.id, certclass)).first;
      this->parse_classes_row(&ins->second, node->children);
    }
    node = node->next;
  }
}

/* ---------------------------------------------------------------- */

void
ApiCertTree::parse_classes_row (ApiCertClass* cclass, xmlNodePtr node)
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
ApiCertTree::parse_certificates_rowset (ApiCertClass* cclass, xmlNodePtr node)
{
  /* Look for row tags. These are for the certificates. */
  while (node != 0)
  {
    if (node->type == XML_ELEMENT_NODE
        && !xmlStrcmp(node->name, (xmlChar const*)"row"))
    {
      ApiCert certificate;
      certificate.class_details = cclass;
      certificate.id = this->get_property_int(node, "certificateID");
      certificate.grade = this->get_property_int(node, "grade");
      certificate.desc = this->get_property(node, "description");

      //std::cout << "Inserting certificate: " << certificate.id << std::endl;
      ApiCertMap::iterator ins = this->certificates.insert
          (std::make_pair(certificate.id, certificate)).first;
      this->parse_certificate_row(&ins->second, node->children);
    }
    node = node->next;
  }
}

/* ---------------------------------------------------------------- */

void
ApiCertTree::parse_certificate_row (ApiCert* cert, xmlNodePtr node)
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
            || xmlStrcmp(cnode->name, (xmlChar const*)"row"))
          continue;

        int skill_id = this->get_property_int(cnode, "typeID");
        int skill_level = this->get_property_int(cnode, "level");
        cert->skilldeps.push_back(std::make_pair(skill_id, skill_level));
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
        cert->certdeps.push_back(std::make_pair(cert_id, cert_grade));
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

char const*
ApiCertTree::get_name_for_grade (int grade)
{
  if (grade <= 0 || grade > 5)
    throw Exception("Error: Invalid certificate grade: "
        + Helpers::get_string_from_int(grade));

  switch (grade)
  {
    case 1: return "Basic";
    case 2: return "Standard";
    case 3: return "Improved";
    case 5: return "Elite";
    default: break;
  }
  return "Error";
}

/* ---------------------------------------------------------------- */

int
ApiCertTree::get_grade_index (int grade)
{
  if (grade <= 0 || grade > 5 || grade == 4)
    return -1;

  if (grade == 5)
    return 3;

  return grade - 1;
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

