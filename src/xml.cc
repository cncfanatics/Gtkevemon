#include "exception.h"
#include "xml.h"

void
XmlDocument::parse (std::string const& data)
{
  xmlFreeDoc(this->doc);

  this->doc = xmlParseMemory(data.c_str(), data.size());
  if (this->doc == 0)
    throw Exception("Document not parsed successfully!");
}

/* ---------------------------------------------------------------- */

xmlNodePtr
XmlDocument::get_root_element (void)
{
  xmlNodePtr cur = xmlDocGetRootElement(this->doc);
  if (!cur)
    throw Exception("Document has no root element!");

  return cur;
}
