#ifndef GUI_ABOUT_DIALOG_HEADER
#define GUI_ABOUT_DIALOG_HEADER

#include <gtkmm/label.h>

#include "asynchttp.h"
#include "winbase.h"

class GuiAboutDialog : public WinBase
{
  private:
    Gtk::Label version_label;
    sigc::connection request;

  public:
    GuiAboutDialog (void);
    ~GuiAboutDialog (void);

    void request_version_label (void);
    void set_version_label (AsyncHttp* data);
};

#endif /* GUI_ABOUT_DIALOG_HEADER */
