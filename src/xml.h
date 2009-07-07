/*
 * This file is part of GtkEveMon.
 *
 * GtkEveMon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with GtkEveMon. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef XML_HEADER
#define XML_HEADER

#include <string>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "ref_ptr.h"

class XmlDocument;
typedef ref_ptr<XmlDocument> XmlDocumentPtr;

class XmlDocument
{
  private:
    xmlDocPtr doc;

  protected:
    XmlDocument (void);

  public:
    static XmlDocumentPtr create (void);
    static XmlDocumentPtr create (std::string const& data);
    static XmlDocumentPtr create (char const* data, std::size_t size);
    static XmlDocumentPtr create_from_file (std::string const& filename);
    ~XmlDocument (void);

    void parse (std::string const& data);
    void parse (char const* data, std::size_t size);
    void parse_from_file (std::string const& filename);
    xmlNodePtr get_root_element (void);
};

/* ---------------------------------------------------------------- */

class XmlBase
{
  protected:
    /* Some helper functions. */
    std::string get_property (xmlNodePtr node, char const* name);
    int get_property_int (xmlNodePtr node, char const* name);
    std::string get_node_text (xmlNodePtr node);

    void set_string_if_node_text (xmlNodePtr node, char const* node_name,
        std::string& target);
    void set_int_if_node_text (xmlNodePtr node, char const* node_name,
        int& target);
    void set_double_if_node_text (xmlNodePtr node, char const* node_name,
        double& target);
    void set_bool_if_node_text (xmlNodePtr node, char const* node_name,
        bool& target);
};

/* ---------------------------------------------------------------- */

inline
XmlDocument::XmlDocument (void)
{
  this->doc = 0;
}

inline XmlDocumentPtr
XmlDocument::create (void)
{
  return XmlDocumentPtr(new XmlDocument);
}

inline XmlDocumentPtr
XmlDocument::create (std::string const& data)
{
  XmlDocumentPtr doc = XmlDocumentPtr(new XmlDocument);
  doc->parse(data);
  return doc;
}

inline XmlDocumentPtr
XmlDocument::create (char const* data, std::size_t size)
{
  XmlDocumentPtr doc = XmlDocumentPtr(new XmlDocument);
  doc->parse(data, size);
  return doc;
}

inline XmlDocumentPtr
XmlDocument::create_from_file (std::string const& filename)
{
  XmlDocumentPtr doc = XmlDocumentPtr(new XmlDocument);
  doc->parse_from_file(filename);
  return doc;
}

inline
XmlDocument::~XmlDocument (void)
{
  xmlFreeDoc(this->doc);
}

#endif /* XML_HEADER */
