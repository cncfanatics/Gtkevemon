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
