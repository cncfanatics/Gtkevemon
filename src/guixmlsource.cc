#include <gtkmm/textbuffer.h>
#include <gtkmm/textview.h>
#include <gtkmm/separator.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>

#include "gtkdefines.h"
#include "guixmlsource.h"

GuiXmlSource::GuiXmlSource (void)
{
  Gtk::Button* close_but = MK_BUT(Gtk::Stock::CLOSE);
  Gtk::HBox* button_box = MK_HBOX;
  button_box->pack_start(*MK_HSEP, true, true, 0);
  button_box->pack_start(*close_but, false, false, 0);

  Gtk::VBox* main_box = MK_VBOX;
  main_box->set_border_width(5);
  main_box->pack_start(this->notebook, true, true, 0);
  main_box->pack_start(*button_box, false, false, 0);

  close_but->signal_clicked().connect(sigc::mem_fun(*this, &WinBase::close));

  this->add(*main_box);
  this->set_default_size(500, 600);
  this->set_title("XML Source - GtkEveMon");
  this->show_all();
}

/* ---------------------------------------------------------------- */

void
GuiXmlSource::append (HttpDataPtr data, std::string const& title)
{
  Glib::RefPtr<Gtk::TextBuffer> buffer = Gtk::TextBuffer::create();

  if (data.get() != 0 && data->size != 0)
    buffer->set_text(Glib::ustring(data->data, data->data + data->size));
  else
    buffer->set_text("There is no data available!");

  Gtk::TextView* textview = Gtk::manage(new Gtk::TextView(buffer));
  textview->set_editable(false);
  Gtk::ScrolledWindow* scwin = MK_SCWIN;
  scwin->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
  scwin->set_border_width(5);
  scwin->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  scwin->add(*textview);
  scwin->show_all();
  this->notebook.append_page(*scwin, title);
}
