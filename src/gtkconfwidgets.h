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

#ifndef GTK_CONF_WIDGETS_HEADER
#define GTK_CONF_WIDGETS_HEADER

#include <string>
#include <gtkmm/filechooserbutton.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/comboboxtext.h>

#include "config.h"

class GtkConfFileChooser : public Gtk::FileChooserButton
{
  private:
    ConfValuePtr value;

    void on_selection_change (void);

  public:
    GtkConfFileChooser (std::string const& key, Gtk::FileChooserAction action);
};

/* ---------------------------------------------------------------- */

class GtkConfCheckButton : public Gtk::CheckButton
{
  private:
    ConfValuePtr value;

    void on_button_toggled (void);

  public:
    GtkConfCheckButton (const Glib::ustring& label, bool mnemonic,
        const std::string& conf_key);
};

/* ---------------------------------------------------------------- */

class GtkConfTextEntry : public Gtk::Entry
{
  private:
    ConfValuePtr value;

    void on_text_changed (void);

  public:
    GtkConfTextEntry (std::string const& conf_key);
};

/* ---------------------------------------------------------------- */

class GtkConfComboBox : public Gtk::ComboBoxText
{
  private:
    ConfValuePtr value;
    std::vector<std::string> values;

    void on_changed_signal (void);

  public:
    GtkConfComboBox (const std::string& conf_key);
    void append_conf_row (const std::string& text, const std::string& value);
};


#endif /* GTK_CONF_WIDGETS_HEADER */
