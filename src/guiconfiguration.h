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

#ifndef GUI_OPTIONS_HEADER
#define GUI_OPTIONS_HEADER

#include <gtkmm/image.h>
#include <gtkmm/label.h>

#include "winbase.h"
#include "gtkconfwidgets.h"

class GuiConfiguration : public WinBase
{
  private:
    GtkConfFileChooser eve_cache;
    GtkConfComboBox tray_usage;
    Gtk::Image valid_icon;
    Gtk::Label valid_label;

    void check_new_path (void);

  public:
    GuiConfiguration (void);

    Glib::SignalProxy0<void> signal_tray_settings_changed (void);
};

/* ---------------------------------------------------------------- */

inline Glib::SignalProxy0<void>
GuiConfiguration::signal_tray_settings_changed (void)
{
  return this->tray_usage.signal_changed();
}

#endif /* GUI_OPTIONS_HEADER */
