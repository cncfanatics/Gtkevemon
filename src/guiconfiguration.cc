#include <sys/stat.h>
#include <iostream>
#include <gtkmm/stock.h>
#include <gtkmm/separator.h>
#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/button.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/notebook.h>

#include "gtkdefines.h"
#include "imagestore.h"
#include "guiconfiguration.h"

GuiConfiguration::GuiConfiguration (void)
  : eve_cache("settings.eve_cache", Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER),
    tray_usage("settings.tray_usage")
{
  /* The MISC notebook tab. */
  GtkConfTextEntry* eve_cmd_entry = Gtk::manage(new GtkConfTextEntry
      ("settings.eve_command"));
  Gtk::HBox* eve_cmd_box = MK_HBOX;
  eve_cmd_box->pack_start(*MK_LABEL("EVE command:"), false, false, 0);
  eve_cmd_box->pack_start(*eve_cmd_entry, true, true, 0);

  //GtkConfCheckButton* misc_tray_cb = Gtk::manage(new GtkConfCheckButton
  //    ("Minimize to tray", false, "settings.minimize_to_tray"));

  this->tray_usage.append_conf_row("Don't use tray", "never");
  this->tray_usage.append_conf_row("Minimize to tray", "minimize");
  this->tray_usage.append_conf_row("Always stay in tray", "always");

  Gtk::HBox* misc_tray_box = MK_HBOX;
  misc_tray_box->pack_start(*MK_LABEL("Tray usage:"), false, false, 0);
  misc_tray_box->pack_start(this->tray_usage, true, true, 0);

  GtkConfCheckButton* misc_min_on_close_cb = Gtk::manage
      (new GtkConfCheckButton("Minimize on close", false,
      "settings.minimize_on_close"));

  Gtk::VBox* page_misc = MK_VBOX;
  page_misc->set_border_width(5);
  page_misc->pack_start(*misc_tray_box, false, false, 0);
  page_misc->pack_start(*misc_min_on_close_cb, false, false, 0);
  page_misc->pack_start(*eve_cmd_box, false, false, 0);

  /* The NETWORK notebook tab. */
  Gtk::Label* net_info_label = MK_LABEL("EVE API requests are done "
      "over the HTTP protocol. If you're behind a restricting firewall, "
      "or just want to stay anonymous, you can use a HTTP proxy server. "
      "Note that the server monitor doesn't use the proxy.");
  net_info_label->property_xalign() = 0.0f;
  net_info_label->set_line_wrap(true);

  GtkConfCheckButton* use_proxy_cb = Gtk::manage(new GtkConfCheckButton
      ("Use the following HTTP proxy", false, "network.use_proxy"));

  Gtk::Label* net_proxy_label = MK_LABEL("Proxy IP:");
  GtkConfTextEntry* proxy_entry = Gtk::manage(new GtkConfTextEntry
      ("network.proxy_address"));
  Gtk::Label* net_proxy_port_label = MK_LABEL("Port:");
  GtkConfTextEntry* proxy_port_entry = Gtk::manage(new GtkConfTextEntry
      ("network.proxy_port"));
  proxy_port_entry->set_width_chars(5);

  Gtk::HBox* net_proxy_entry_box = MK_HBOX;
  net_proxy_entry_box->pack_start(*net_proxy_label, false, false, 0);
  net_proxy_entry_box->pack_start(*proxy_entry, true, true, 0);
  net_proxy_entry_box->pack_start(*net_proxy_port_label, false, false, 0);
  net_proxy_entry_box->pack_start(*proxy_port_entry, false, false, 0);

  Gtk::VBox* page_network = MK_VBOX;
  page_network->set_border_width(5);
  page_network->pack_start(*net_info_label, false, false, 0);
  page_network->pack_start(*use_proxy_cb, false, false, 0);
  page_network->pack_start(*net_proxy_entry_box, false, false, 0);

  /* The NOTIFICATIONS notebook tab. */
  Gtk::Label* notify_info_label = MK_LABEL("Check the notification "
      "options in order to choose how GtkEveMon will notify you "
      "if the skill training is complete.");
  notify_info_label->property_xalign() = 0.0f;
  notify_info_label->set_line_wrap(true);

  GtkConfCheckButton* notify_with_popup = Gtk::manage
      (new GtkConfCheckButton("Show popup dialog", false,
      "notifications.show_popup_dialog"));
  GtkConfCheckButton* notify_with_tray = Gtk::manage
      (new GtkConfCheckButton("Show tray icon", false,
      "notifications.show_tray_icon"));

  Gtk::VBox* page_notifications = MK_VBOX;
  page_notifications->set_border_width(5);
  page_notifications->pack_start(*notify_info_label, false, false, 0);
  page_notifications->pack_start(*notify_with_popup, false, false, 0);
  page_notifications->pack_start(*notify_with_tray, false, false, 0);

  /* The DIRECTORIES notebook tab. */
  this->valid_label.property_xalign() = 0.0f;

  Gtk::Label* dir_info_label = MK_LABEL("Enter the directory where "
      "your EVE cache is located. This path will be used to "
      "retreive character portraits. You don't really need to set this "
      "because portraits are fetched online.");
  dir_info_label->property_xalign() = 0.0f;
  dir_info_label->set_line_wrap(true);

  Gtk::HBox* path_hbox = MK_HBOX;
  path_hbox->pack_start(*MK_LABEL("EVE cache:"), false, false, 0);
  path_hbox->pack_start(this->eve_cache, true, true, 0);

  Gtk::HBox* valid_frame_hbox = MK_HBOX;
  valid_frame_hbox->set_border_width(5);
  valid_frame_hbox->pack_start(this->valid_icon, false, false, 0);
  valid_frame_hbox->pack_start(this->valid_label, true, true, 0);

  Gtk::Frame* valid_frame = MK_FRAME0;
  valid_frame->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  valid_frame->add(*valid_frame_hbox);

  Gtk::VBox* page_directories = MK_VBOX;
  page_directories->set_border_width(5);
  page_directories->pack_start(*dir_info_label, false, false, 0);
  page_directories->pack_start(*path_hbox, false, false, 0);
  page_directories->pack_end(*valid_frame, false, false, 0);

  Gtk::HBox* button_bar = MK_HBOX;
  Gtk::Button* close_but = MK_BUT(Gtk::Stock::CLOSE);
  button_bar->pack_end(*close_but, false, false, 0);
  //button_bar->pack_start(*MK_HSEP, true, true, 0);

  /* Notebook packing and window stuff. */
  Gtk::Notebook* notebook = Gtk::manage(new Gtk::Notebook);
  notebook->append_page(*page_directories, "Directories");
  notebook->append_page(*page_notifications, "Notifications");
  notebook->append_page(*page_network, "Network");
  notebook->append_page(*page_misc, "Misc");

  Gtk::VBox* main_vbox = MK_VBOX;
  main_vbox->pack_start(*notebook, true, true, 0);
  main_vbox->pack_end(*button_bar, false, false, 0);

  Gtk::Frame* image_frame = MK_FRAME0;
  image_frame->set_shadow_type(Gtk::SHADOW_IN);
  image_frame->add(*Gtk::manage(new Gtk::Image(ImageStore::aboutlogo)));

  Gtk::HBox* main_hbox = MK_HBOX;
  main_hbox->set_border_width(5);
  main_hbox->pack_start(*image_frame, false, false, 0);
  main_hbox->pack_start(*main_vbox, true, true, 0);

  this->add(*main_hbox);
  this->set_title("Configuration - GtkEveMon");
  this->show_all();

  this->check_new_path();

  close_but->signal_clicked().connect(sigc::mem_fun(*this, &WinBase::close));
  this->eve_cache.signal_selection_changed().connect(sigc::mem_fun
      (*this, &GuiConfiguration::check_new_path));
}

/* ---------------------------------------------------------------- */

void
GuiConfiguration::check_new_path (void)
{
  Glib::ustring path = this->eve_cache.get_filename();
  Glib::ustring pics = path + "/Pictures";
  Glib::ustring ports = pics + "/Portraits";

  //std::cout << "New path is: " << path << std::endl;

  struct stat finfo;
  if (::stat(pics.c_str(), &finfo) < 0 || ::stat(ports.c_str(), &finfo) < 0)
  {
    this->valid_label.set_text("The path seems to be INVALID!");
    this->valid_icon.set(Gtk::Stock::NO, Gtk::ICON_SIZE_BUTTON);
  }
  else
  {
    this->valid_label.set_text("The path seems to be VALID!");
    this->valid_icon.set(Gtk::Stock::YES, Gtk::ICON_SIZE_BUTTON);
  }
}
