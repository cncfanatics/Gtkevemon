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
    ~XmlDocument (void);

    void parse (std::string const& data);
    xmlNodePtr get_root_element (void);
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

inline
XmlDocument::~XmlDocument (void)
{
  xmlFreeDoc(this->doc);
}

#endif /* XML_HEADER */
