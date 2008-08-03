#ifndef GUI_XML_SOURCE_HEADER
#define GUI_XML_SOURCE_HEADER

#include <string.h>
#include <gtkmm/notebook.h>

#include "winbase.h"
#include "http.h"

class GuiXmlSource : public WinBase
{
  private:
    Gtk::Notebook notebook;

  public:
    GuiXmlSource (void);
    void append (HttpDataPtr data, std::string const& title);
};

#endif /* GUI_XML_SOURCE_HEADER */
