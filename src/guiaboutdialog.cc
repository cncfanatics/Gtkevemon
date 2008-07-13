#include <iostream>
#include <gtkmm/separator.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/image.h>
#include <gtkmm/stock.h>
#include <gtkmm/frame.h>
#include <gtkmm/table.h>

#include "asynchttp.h"
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

  this->version_status_image.set(Gtk::Stock::REFRESH, Gtk::ICON_SIZE_BUTTON);

  Gtk::Label* local_label = MK_LABEL("Local version:");
  Gtk::Label* current_label = MK_LABEL("Current version:");
  Gtk::Label* local_label_val = MK_LABEL(GTKEVEMON_VERSION_STR);
  this->version_label.set_text("Requesting version...");

  local_label->property_xalign() = 0.0f;
  current_label->property_xalign() = 0.0f;
  local_label_val->property_xalign() = 0.0f;
  this->version_label.property_xalign() = 0.0f;

  Gtk::Table* version_table = Gtk::manage(new Gtk::Table(2, 3));
  version_table->set_col_spacings(5);
  version_table->attach(this->version_status_image, 0, 1, 0, 2,
      Gtk::SHRINK, Gtk::SHRINK, 5);
  version_table->attach(*local_label, 1, 2, 0, 1, Gtk::SHRINK|Gtk::FILL);
  version_table->attach(*current_label, 1, 2, 1, 2, Gtk::SHRINK|Gtk::FILL);
  version_table->attach(*local_label_val, 2, 3, 0, 1, Gtk::SHRINK|Gtk::FILL);
  version_table->attach(this->version_label, 2, 3, 1, 2, Gtk::SHRINK|Gtk::FILL);

  Gtk::Label* title_label = MK_LABEL0;
  title_label->set_justify(Gtk::JUSTIFY_LEFT);
  title_label->property_xalign() = 0.0f;
  title_label->set_text("<b>GtkEveMon - a skill monitor for Linux</b>");
  title_label->set_use_markup(true);

  Gtk::Label* info_label = MK_LABEL0;
  info_label->set_line_wrap(true);
  info_label->set_justify(Gtk::JUSTIFY_LEFT);
  info_label->property_xalign() = 0.0f;
  info_label->set_text(
      "GtkEveMon is a skill monitoring standalone\n"
      "application for GNU/Linux systems. With GtkEveMon\n"
      "you can monitor your current skills and your\n"
      "skill training process without starting EVE-Online.\n"
      "You will never miss to train your next skill!\n"
      "\n"
      "Homepage: http://gtkevemon.battleclinic.com\n"
      "Forum: http://www.battleclinic.com/forum/index.php#43\n"
      "\n"
      "<i>Simon Fuhrmann &lt;SimonFuhrmann@gmx.de&gt;</i>");
  info_label->set_use_markup(true);

  Gtk::Button* close_but = MK_BUT(Gtk::Stock::CLOSE);
  Gtk::HBox* button_box = MK_HBOX;
  button_box->pack_end(*close_but, false, false, 0);

  Gtk::VBox* about_label_box = MK_VBOX;
  about_label_box->pack_start(*title_label, false, false, 0);
  about_label_box->pack_start(*info_label, false, false, 0);
  about_label_box->pack_end(*button_box, false, false, 0);
  about_label_box->pack_end(*MK_HSEP, false, false, 0);
  about_label_box->pack_end(*version_table, false, false, 0);

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

  title_label->set_selectable(true);
  this->version_label.set_selectable(true);
  info_label->set_selectable(true);

  this->request_version_label();
}

/* ---------------------------------------------------------------- */

GuiAboutDialog::~GuiAboutDialog (void)
{
  this->request.disconnect();
}

/* ---------------------------------------------------------------- */

void
GuiAboutDialog::request_version_label (void)
{
  AsyncHttp* http = AsyncHttp::create();
  http->set_host("gtkevemon.battleclinic.com");
  http->set_path("/svn_version.txt");
  http->set_agent("GtkEveMon");
  this->request = http->signal_done().connect(sigc::mem_fun
      (*this, &GuiAboutDialog::set_version_label));
  http->async_request();
}

/* ---------------------------------------------------------------- */

void
GuiAboutDialog::set_version_label (AsyncHttpData result)
{
  if (result.data.get() == 0)
  {
    this->version_label.set_text("Error fetchting version!");
    this->version_status_image.set(Gtk::Stock::NO, Gtk::ICON_SIZE_BUTTON);
    std::cout << "Could not fetch version: " << result.exception << std::endl;
  }
  else
  {
    this->version_label.set_text(result.data->data);
    if (std::string(result.data->data) != GTKEVEMON_VERSION_STR)
      this->version_status_image.set(Gtk::Stock::NO, Gtk::ICON_SIZE_BUTTON);
    else
      this->version_status_image.set(Gtk::Stock::YES, Gtk::ICON_SIZE_BUTTON);
  }
}
