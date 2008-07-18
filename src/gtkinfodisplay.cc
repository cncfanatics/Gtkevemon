#include <gtkmm/scrolledwindow.h>
#include <gtkmm/button.h>
#include <gtkmm/separator.h>
#include <gtkmm/stock.h>

#include "gtkdefines.h"
#include "gtkinfodisplay.h"

GtkInfoDisplay::GtkInfoDisplay (void)
  : Gtk::VBox(false, 0)
{
  this->text.property_xalign() = 0.0f;

  this->info_but.set_image(*Gtk::manage(new Gtk::Image
        (Gtk::Stock::FIND, Gtk::ICON_SIZE_MENU)));
  this->info_but.set_relief(Gtk::RELIEF_NONE);
  Gtk::Button* close_but = MK_BUT0;
  close_but->set_image(*Gtk::manage(new Gtk::Image
        (Gtk::Stock::CLOSE, Gtk::ICON_SIZE_MENU)));
  close_but->set_relief(Gtk::RELIEF_NONE);

  Gtk::HBox* button_box = MK_HBOX0;
  button_box->pack_start(this->info_but, false, false, 0);
  button_box->pack_start(*close_but, false, false, 0);

  Gtk::HBox* item_box = MK_HBOX;
  item_box->pack_start(this->icon, false, false, 0);
  item_box->pack_start(this->text, true, true, 0);
  item_box->pack_start(*button_box, false, false, 0);

  this->pack_start(*MK_HSEP, false, false, 0);
  this->pack_start(*item_box, false, false, 0);

  close_but->signal_clicked().connect(sigc::mem_fun
      (*this, &Gtk::Widget::hide));
  this->info_but.signal_clicked().connect(sigc::mem_fun
      (*this, &GtkInfoDisplay::show_info_log));
}

/* ---------------------------------------------------------------- */

void
GtkInfoDisplay::append (InfoItem const& item)
{
  this->text.set_text(Glib::locale_to_utf8(item.message));
  this->text.set_use_markup(true);

  switch (item.type)
  {
    case INFO_NOTIFICATION:
      this->icon.set(Gtk::Stock::DIALOG_INFO, Gtk::ICON_SIZE_BUTTON);
      break;
    case INFO_WARNING:
      this->icon.set(Gtk::Stock::DIALOG_WARNING, Gtk::ICON_SIZE_BUTTON);
      break;
    default:
    case INFO_ERROR:
      this->icon.set(Gtk::Stock::DIALOG_ERROR, Gtk::ICON_SIZE_BUTTON);
      break;
  }

  if (item.details.empty())
    this->info_but.set_sensitive(false);
  else
    this->info_but.set_sensitive(true);

  this->log.push_back(item);
  this->show();
}

/* ---------------------------------------------------------------- */

void
GtkInfoDisplay::show_info_log (void)
{
  GtkInfoDisplayLog* log = new GtkInfoDisplayLog(this->log.back());

  Gtk::Window* toplevel = (Gtk::Window*)this->get_toplevel();
  log->set_transient_for(*toplevel);

  this->hide();
}

/* ---------------------------------------------------------------- */

GtkInfoDisplayLog::GtkInfoDisplayLog (InfoItem const& item)
  : text_buffer(Gtk::TextBuffer::create()), text_view(text_buffer)
{
  Gtk::Button* close_but = MK_BUT(Gtk::Stock::CLOSE);

  Gtk::HBox* button_box = MK_HBOX;
  button_box->pack_start(*MK_HSEP, true, true, 0);
  button_box->pack_start(*close_but, false, false, 0);

  Gtk::ScrolledWindow* scwin = MK_SCWIN;
  scwin->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
  scwin->add(this->text_view);
  scwin->set_shadow_type(Gtk::SHADOW_ETCHED_IN);

  this->text_view.set_editable(false);
  this->text_view.set_wrap_mode(Gtk::WRAP_WORD);

  this->text_buffer->set_text(Glib::locale_to_utf8(item.message)
      + "\n\n" + Glib::locale_to_utf8(item.details));

  Gtk::VBox* main_box = MK_VBOX;
  main_box->set_border_width(5);
  main_box->pack_start(*scwin, true, true, 0);
  main_box->pack_end(*button_box, false, false, 0);

  this->add(*main_box);
  this->set_title("Info Log - GtkEveMon");

  close_but->signal_clicked().connect(sigc::mem_fun(*this, &WinBase::close));

  this->set_default_size(350, 200);
  this->show_all();
}
