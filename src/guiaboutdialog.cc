#include <gtkmm/separator.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/image.h>
#include <gtkmm/stock.h>
#include <gtkmm/frame.h>

#include "defines.h"
#include "gtkdefines.h"
#include "imagestore.h"
#include "guiaboutdialog.h"

GuiAboutDialog::GuiAboutDialog (void)
{
  Gtk::Image* logo = Gtk::manage(new Gtk::Image(ImageStore::aboutlogo));
  Gtk::Frame* logo_frame = MK_FRAME0;
  logo_frame->add(*logo);
  logo_frame->set_shadow_type(Gtk::SHADOW_IN);

  Gtk::Label* about_label = Gtk::manage(new Gtk::Label);
  about_label->set_line_wrap(true);
  about_label->set_justify(Gtk::JUSTIFY_LEFT);
  about_label->set_text("<b>GtkEveMon - a skill monitor for Linux</b>\n"
      GTKEVEMON_VERSION_STR "\n"
      "\n"
      "GtkEveMon is a skill monitoring standalone\n"
      "application for GNU/Linux systems. With GtkEveMon\n"
      "you can monitor your current skills and your\n"
      "skill training process without starting EVE-Online.\n"
      "You will never miss to train your next skill!\n"
      "\n"
      "Homepage: http://tinyurl.com/5na96b\n"
      "Forum-Thread: http://tinyurl.com/6jdpts\n"
      "\n"
      "<i>Simon Fuhrmann &lt;SimonFuhrmann@gmx.de&gt;</i>");
  about_label->set_use_markup(true);

  Gtk::Button* close_but = MK_BUT(Gtk::Stock::CLOSE);
  Gtk::HBox* button_box = MK_HBOX;
  button_box->pack_end(*close_but, false, false, 0);

  Gtk::VBox* about_label_box = MK_VBOX;
  about_label_box->pack_start(*about_label, false, false, 0);
  about_label_box->pack_end(*button_box, false, false, 0);
  about_label_box->pack_end(*MK_HSEP, false, false, 0);

  Gtk::HBox* logo_text_box = MK_HBOX;
  logo_text_box->set_spacing(10);
  logo_text_box->pack_start(*logo_frame, false, false, 0);
  logo_text_box->pack_start(*about_label_box, true, true, 0);

  Gtk::VBox* main_box = MK_VBOX;
  main_box->set_border_width(5);
  main_box->pack_start(*logo_text_box, false, false, 0);

  close_but->signal_clicked().connect(sigc::mem_fun(*this, &WinBase::close));

  this->add(*main_box);
  this->set_default_size(410, 250);
  this->set_title("About GtkEveMon");
  this->show_all();

  about_label->set_selectable(true);
}
