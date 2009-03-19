#include <gtkmm/button.h>
#include <gtkmm/box.h>
#include <gtkmm/separator.h>
#include <gtkmm/frame.h>
#include <gtkmm/stock.h>

#include "gtkdefines.h"
#include "guiskillqueue.h"

GuiSkillQueue::GuiSkillQueue (EveApiAuth const& auth)
{
  Gtk::Frame* main_frame = MK_FRAME0;
  main_frame->add(this->queue);

  Gtk::Button* refresh_but = MK_BUT(Gtk::Stock::REFRESH);
  Gtk::Button* close_but = MK_BUT(Gtk::Stock::CLOSE);

  Gtk::HBox* button_box = MK_HBOX;
  button_box->pack_start(*refresh_but, false, false, 0);
  button_box->pack_start(*MK_HSEP, true, true, 0);
  button_box->pack_start(*close_but, false, false, 0);

  Gtk::VBox* main_box = MK_VBOX;
  main_box->set_border_width(5);
  main_box->pack_start(*main_frame, true, true, 0);
  main_box->pack_start(*button_box, false, false, 0);

  close_but->signal_clicked().connect(sigc::mem_fun
      (*this, &WinBase::close));
  refresh_but->signal_clicked().connect(sigc::mem_fun
      (this->queue, &GtkSkillQueue::refresh));

  this->add(*main_box);
  this->set_default_size(450, 300);
  this->set_title("Training queue - GtkEveMon");
  this->show_all();

  this->queue.set_auth_data(auth);
  this->queue.refresh();
}
