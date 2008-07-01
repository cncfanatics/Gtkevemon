#include "gtkportrait.h"
#include "config.h"
#include "gtkconfwidgets.h"

GtkConfFileChooser::GtkConfFileChooser (std::string const& key,
    Gtk::FileChooserAction action)
  : Gtk::FileChooserButton(action)
{
  this->value = Config::conf.get_value(key);

  if ((**this->value).empty())
    this->set_filename("/");
  else
    this->set_filename(**this->value);

  this->signal_selection_changed().connect(sigc::mem_fun
      (*this, &GtkConfFileChooser::on_selection_change));
}

/* ---------------------------------------------------------------- */

void
GtkConfFileChooser::on_selection_change (void)
{
  this->value->set(this->get_filename());
  GtkPortrait::emit_update_signal();
}

/* ================================================================ */

GtkConfCheckButton::GtkConfCheckButton (const Glib::ustring& label,
    bool mnemonic, const std::string& conf_key)
  : Gtk::CheckButton(label, mnemonic)
{
  this->value = Config::conf.get_value(conf_key);
  this->set_active(this->value->get_bool());

  this->signal_toggled().connect(sigc::mem_fun
      (*this, &GtkConfCheckButton::on_button_toggled));
}

/* ---------------------------------------------------------------- */

void
GtkConfCheckButton::on_button_toggled (void)
{
  this->value->set(this->get_active());
}

/* ================================================================ */

GtkConfTextEntry::GtkConfTextEntry (std::string const& conf_key)
{
  this->value = Config::conf.get_value(conf_key);
  this->set_text(value->get_string());

  this->signal_changed().connect(sigc::mem_fun
      (*this, &GtkConfTextEntry::on_text_changed));
}

/* ---------------------------------------------------------------- */

void
GtkConfTextEntry::on_text_changed (void)
{
  this->value->set(this->get_text());
}

/* ================================================================ */

GtkConfComboBox::GtkConfComboBox (const std::string& conf_key)
{
  this->value = Config::conf.get_value(conf_key);
  this->signal_changed().connect(sigc::mem_fun(*this,
      &GtkConfComboBox::on_changed_signal));
}

/* ---------------------------------------------------------------- */

void
GtkConfComboBox::on_changed_signal (void)
{
  this->value->set(this->values[this->get_active_row_number()]);
}

/* ---------------------------------------------------------------- */

void
GtkConfComboBox::append_conf_row (const std::string& text,
    const std::string& value)
{
  this->values.push_back(value);
  this->append_text(text);

  if (value == this->value->get_string())
    this->set_active(this->values.size() - 1);
}
