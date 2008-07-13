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
    Gtk::Image version_status_image;

  public:
    GuiAboutDialog (void);
    ~GuiAboutDialog (void);

    void request_version_label (void);
    void set_version_label (AsyncHttpData result);
};

#endif /* GUI_ABOUT_DIALOG_HEADER */
