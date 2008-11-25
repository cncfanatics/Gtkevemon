#include <sys/stat.h>
#include <iostream>
#include <gtkmm/table.h>
#include <gtkmm/stock.h>
#include <gtkmm/separator.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/button.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/textview.h>
#include <gtkmm/notebook.h>

#include "defines.h"
#include "helpers.h"
#include "gtkdefines.h"
#include "imagestore.h"
#include "guiconfiguration.h"

GuiConfiguration::GuiConfiguration (void)
  : tray_usage("settings.tray_usage")
{
  /* The MISC notebook tab. */
  this->tray_usage.append_conf_row("Don't use tray", "never");
  this->tray_usage.append_conf_row("Minimize to tray", "minimize");
  this->tray_usage.append_conf_row("Always stay in tray", "always");

  Gtk::HBox* misc_tray_box = MK_HBOX;
  misc_tray_box->pack_start(*MK_LABEL("Tray usage:"), false, false, 0);
  misc_tray_box->pack_start(this->tray_usage, true, true, 0);

  GtkConfCheckButton* misc_min_on_close_cb = Gtk::manage
      (new GtkConfCheckButton("Minimize on close", false,
      "settings.minimize_on_close"));
  GtkConfCheckButton* misc_detailed_tray_tt_cb = Gtk::manage
      (new GtkConfCheckButton("Detailed tray tooltip",
      false, "settings.detailed_tray_tooltip"));
  GtkConfCheckButton* misc_autoupdate_sheets_cb = Gtk::manage
      (new GtkConfCheckButton("Automatically update character status",
      false, "settings.auto_update_sheets"));
  GtkConfCheckButton* misc_svn_versioncheck_cb = Gtk::manage
      (new GtkConfCheckButton("Notify about new GtkEveMon SVN versions",
      false, "versionchecker.svn_check"));
  /*
  GtkConfCheckButton* misc_data_versioncheck_cb = Gtk::manage
      (new GtkConfCheckButton("Notify about new data file versions",
      false, "versionchecker.data_check"));
  */
  GtkConfCheckButton* misc_verbose_wintitle_cb = Gtk::manage
      (new GtkConfCheckButton("Show training info in window title",
      false, "settings.verbose_wintitle"));

  Gtk::VBox* misc_cb_box = MK_VBOX0;
  misc_cb_box->pack_start(*misc_min_on_close_cb, false, false, 0);
  misc_cb_box->pack_start(*misc_detailed_tray_tt_cb, false, false, 0);
  misc_cb_box->pack_start(*misc_autoupdate_sheets_cb, false, false, 0);
  misc_cb_box->pack_start(*misc_svn_versioncheck_cb, false, false, 0);
  //misc_cb_box->pack_start(*misc_data_versioncheck_cb, false, false, 0);
  misc_cb_box->pack_start(*misc_verbose_wintitle_cb, false, false, 0);

  Gtk::VBox* page_misc = MK_VBOX;
  page_misc->set_border_width(5);
  page_misc->pack_start(*misc_tray_box, false, false, 0);
  page_misc->pack_start(*misc_cb_box, false, false, 0);

  /* The LAUNCHER notebook tab. */
  Gtk::Label* launch_info_label = MK_LABEL("You can define up to "
      "five EVE commands to launch EVE online. If you specify more than "
      "one command, GtkEveMon will prompt for the command to be started.\n"
      "Note that \"~\" will not work as home directory.");
  launch_info_label->property_xalign() = 0.0f;
  launch_info_label->set_line_wrap(true);

  Gtk::Table* launch_table = Gtk::manage(new Gtk::Table
      (LAUNCHER_CMD_AMOUNT, 2, false));
  launch_table->set_col_spacings(5);
  launch_table->set_row_spacings(1);

  for (unsigned int i = 0; i < LAUNCHER_CMD_AMOUNT; ++i)
  {
    std::string key;
    if (i == 0)
      key = "eve_command";
    else
      key = "eve_command_" + Helpers::get_string_from_uint(i + 1);

    Gtk::Label* eve_cmd_label = MK_LABEL("Command "
        + Helpers::get_string_from_uint(i + 1) + ":");
    eve_cmd_label->property_xalign() = 0.0f;

    GtkConfTextEntry* eve_cmd_entry = Gtk::manage
        (new GtkConfTextEntry("settings." + key));

    launch_table->attach(*eve_cmd_label, 0, 1, i, i + 1, Gtk::FILL);
    launch_table->attach(*eve_cmd_entry, 1, 2, i, i + 1, Gtk::EXPAND|Gtk::FILL);
  }

  Gtk::VBox* page_launch = MK_VBOX;
  page_launch->set_border_width(5);
  page_launch->pack_start(*launch_info_label, false, false, 0);
  page_launch->pack_start(*launch_table, false, false, 0);

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
  GtkConfCheckButton* notify_with_info_bar = Gtk::manage
      (new GtkConfCheckButton("Show information bar", false,
      "notifications.show_info_bar"));

  Gtk::VBox* notify_cb_box = MK_VBOX0;
  notify_cb_box->pack_start(*notify_with_popup, false, false, 0);
  notify_cb_box->pack_start(*notify_with_tray, false, false, 0);
  notify_cb_box->pack_start(*notify_with_info_bar, false, false, 0);

  Gtk::Label* notify_info2_label = MK_LABEL("See the forums for "
      "how to send emails with this handler.");
  notify_info2_label->property_xalign() = 0.0f;
  notify_info2_label->set_line_wrap(true);

  GtkConfCheckButton* notify_handler_enabled = Gtk::manage
      (new GtkConfCheckButton("Enable notification handler", false,
      "notifications.exec_handler"));
  GtkConfTextEntry* notify_command_entry = Gtk::manage
      (new GtkConfTextEntry("notifications.exec_command"));
  GtkConfTextEntry* notify_data_entry = Gtk::manage
      (new GtkConfTextEntry("notifications.exec_data"));
  GtkConfTextEntry* notify_min_sp_entry = Gtk::manage
      (new GtkConfTextEntry("notifications.minimum_sp"));

  Gtk::Label* notify_command_label = MK_LABEL("Command to execute:");
  notify_command_label->property_xalign() = 0.0;
  Gtk::Label* notify_data_label = MK_LABEL("Data to send (stdin):");
  notify_data_label->property_xalign() = 0.0;
  Gtk::Label* notify_minsp_label = MK_LABEL("Minimum skill SP:");
  notify_minsp_label->property_xalign() = 0.0;

  Gtk::Table* notify_handler_table = Gtk::manage(new Gtk::Table(4, 2));
  notify_handler_table->set_row_spacings(1);
  notify_handler_table->set_col_spacings(5);
  notify_handler_table->attach(*notify_handler_enabled, 0, 2, 0, 1,
      Gtk::EXPAND | Gtk::FILL);
  notify_handler_table->attach(*notify_command_label, 0, 1, 1, 2,
      Gtk::SHRINK | Gtk::FILL);
  notify_handler_table->attach(*notify_data_label, 0, 1, 2, 3,
      Gtk::SHRINK | Gtk::FILL);
  notify_handler_table->attach(*notify_minsp_label, 0, 1, 3, 4,
      Gtk::SHRINK | Gtk::FILL);
  notify_handler_table->attach(*notify_command_entry, 1, 2, 1, 2,
      Gtk::EXPAND | Gtk::FILL);
  notify_handler_table->attach(*notify_data_entry, 1, 2, 2, 3,
      Gtk::EXPAND | Gtk::FILL);
  notify_handler_table->attach(*notify_min_sp_entry, 1, 2, 3, 4,
      Gtk::EXPAND | Gtk::FILL);

  Gtk::VBox* page_notifications = MK_VBOX;
  page_notifications->set_border_width(5);
  page_notifications->pack_start(*notify_info_label, false, false, 0);
  page_notifications->pack_start(*notify_cb_box, false, false, 0);
  page_notifications->pack_start(*notify_info2_label, false, false, 0);
  page_notifications->pack_start(*notify_handler_table, false, false, 0);

  /* The TIME_FORMAT notebook tab. */
  Gtk::Label* time_info_label = MK_LABEL
      ("Enter your desired time format here.");
  time_info_label->property_xalign() = 0.0f;
  time_info_label->set_line_wrap(true);

  Gtk::Label* time_info_label2 = MK_LABEL
      ("The default time format is: %Y-%m-%d %H:%M:%S");
  time_info_label2->property_xalign() = 0.0f;
  time_info_label2->set_selectable(true);

  GtkConfTextEntry* time_format_entry = Gtk::manage
      (new GtkConfTextEntry("evetime.time_format"));

  Gtk::Label* time_info_label3 = MK_LABEL
      ("The default short time format is: %m-%d %H:%M");
  time_info_label3->property_xalign() = 0.0f;
  time_info_label3->set_selectable(true);

  GtkConfTextEntry* time_short_format_entry = Gtk::manage
      (new GtkConfTextEntry("evetime.time_short_format"));

  Glib::RefPtr<Gtk::TextBuffer> time_helptextbuf = Gtk::TextBuffer::create();
  Gtk::TextView* time_helptext = Gtk::manage
      (new Gtk::TextView(time_helptextbuf));
  time_helptext->set_editable(false);
  time_helptextbuf->set_text(
"`%a'\n"
"   The abbreviated weekday name according to the current locale.\n"
"\n"
"`%A'\n"
"   The full weekday name according to the current locale.\n"
"\n"
"`%b'\n"
"   The abbreviated month name according to the current locale.\n"
"\n"
"`%B'\n"
"   The full month name according to the current locale.\n"
"\n"
"   Using `%B' together with `%d' produces grammatically\n"
"   incorrect results for some locales.\n"
"\n"
"`%c'\n"
"   The preferred calendar time representation for the current\n"
"   locale.\n"
"\n"
"`%C'\n"
"   The century of the year. This is equivalent to the greatest\n"
"   integer not greater than the year divided by 100.\n"
"\n"
"   This format was first standardized by POSIX.2-1992 and by\n"
"   ISO C99.\n"
"\n"
"`%d'\n"
"   The day of the month as a decimal number (range `01' through\n"
"   `31').\n"
"\n"
"`%D'\n"
"   The date using the format `%m/%d/%y'.\n"
"\n"
"   This format was first standardized by POSIX.2-1992 and by\n"
"   ISO C99.\n"
"\n"
"`%e'\n"
"   The day of the month like with `%d', but padded with blank\n"
"   (range ` 1' through `31').\n"
"\n"
"   This format was first standardized by POSIX.2-1992 and by\n"
"   ISO C99.\n"
"\n"
"`%F'\n"
"   The date using the format `%Y-%m-%d'. This is the form\n"
"   specified in the ISO 8601 standard and is the preferred form\n"
"   for all uses.\n"
"\n"
"   This format was first standardized by ISO C99 and by\n"
"   POSIX.1-2001.\n"
"\n"
"`%g'\n"
"   The year corresponding to the ISO week number, but without\n"
"   the century (range `00' through `99'). This has the same\n"
"   format and value as `%y', except that if the ISO week number\n"
"   (see `%V') belongs to the previous or next year, that year is\n"
"   used instead.\n"
"\n"
"   This format was first standardized by ISO C99 and by\n"
"   POSIX.1-2001.\n"
"\n"
"`%G'\n"
"   The year corresponding to the ISO week number. This has the\n"
"   same format and value as `%Y', except that if the ISO week\n"
"   number (see `%V') belongs to the previous or next year, that\n"
"   year is used instead.\n"
"\n"
"   This format was first standardized by ISO C99 and by\n"
"   POSIX.1-2001 but was previously available as a GNU extension.\n"
"\n"
"`%h'\n"
"   The abbreviated month name according to the current locale.\n"
"   The action is the same as for `%b'.\n"
"\n"
"   This format was first standardized by POSIX.2-1992 and by\n"
"   ISO C99.\n"
"\n"
"`%H'\n"
"   The hour as a decimal number, using a 24-hour clock (range\n"
"   `00' through `23').\n"
"\n"
"`%I'\n"
"   The hour as a decimal number, using a 12-hour clock (range\n"
"   `01' through `12').\n"
"\n"
"`%j'\n"
"   The day of the year as a decimal number (range `001' through\n"
"   `366').\n"
"\n"
"`%k'\n"
"   The hour as a decimal number, using a 24-hour clock like\n"
"   `%H', but padded with blank (range ` 0' through `23').\n"
"\n"
"   This format is a GNU extension.\n"
"\n"
"`%l'\n"
"   The hour as a decimal number, using a 12-hour clock like\n"
"   `%I', but padded with blank (range ` 1' through `12').\n"
"\n"
"   This format is a GNU extension.\n"
"\n"
"`%m'\n"
"   The month as a decimal number (range `01' through `12').\n"
"\n"
"`%M'\n"
"   The minute as a decimal number (range `00' through `59').\n"
"\n"
"`%n'\n"
"   A single `\\n' (newline) character.\n"
"\n"
"   This format was first standardized by POSIX.2-1992 and by\n"
"   ISO C99.\n"
"\n"
"`%p'\n"
"   Either `AM' or `PM', according to the given time value; or the\n"
"   corresponding strings for the current locale. Noon is\n"
"   treated as `PM' and midnight as `AM'. In most locales\n"
"   `AM'/`PM' format is not supported, in such cases `\"%p\"'\n"
"   yields an empty string.\n"
"\n"
"`%P'\n"
"   Either `am' or `pm', according to the given time value; or the\n"
"   corresponding strings for the current locale, printed in\n"
"   lowercase characters. Noon is treated as `pm' and midnight\n"
"   as `am'. In most locales `AM'/`PM' format is not supported,\n"
"   in such cases `\"%P\"' yields an empty string.\n"
"\n"
"   This format is a GNU extension.\n"
"\n"
"`%r'\n"
"   The complete calendar time using the AM/PM format of the\n"
"   current locale.\n"
"\n"
"   This format was first standardized by POSIX.2-1992 and by\n"
"   ISO C99. In the POSIX locale, this format is equivalent to\n"
"   `%I:%M:%S %p'.\n"
"\n"
"`%R'\n"
"   The hour and minute in decimal numbers using the format\n"
"   `%H:%M'.\n"
"\n"
"   This format was first standardized by ISO C99 and by\n"
"   POSIX.1-2001 but was previously available as a GNU extension.\n"
"\n"
"`%s'\n"
"   The number of seconds since the epoch, i.e., since 1970-01-01\n"
"   00:00:00 UTC. Leap seconds are not counted unless leap\n"
"   second support is available.\n"
"\n"
"   This format is a GNU extension.\n"
"\n"
"`%S'\n"
"   The seconds as a decimal number (range `00' through `60').\n"
"\n"
"`%t'\n"
"   A single `\\t' (tabulator) character.\n"
"\n"
"   This format was first standardized by POSIX.2-1992 and by\n"
"   ISO C99.\n"
"\n"
"`%T'\n"
"   The time of day using decimal numbers using the format\n"
"   `%H:%M:%S'.\n"
"\n"
"   This format was first standardized by POSIX.2-1992 and by\n"
"   ISO C99.\n"
"\n"
"`%u'\n"
"   The day of the week as a decimal number (range `1' through\n"
"   `7'), Monday being `1'.\n"
"\n"
"   This format was first standardized by POSIX.2-1992 and by\n"
"   ISO C99.\n"
"\n"
"`%U'\n"
"   The week number of the current year as a decimal number\n"
"   (range `00' through `53'), starting with the first Sunday as\n"
"   the first day of the first week. Days preceding the first\n"
"   Sunday in the year are considered to be in week `00'.\n"
"\n"
"\n"
"`%V'\n"
"   The ISO 8601:1988 week number as a decimal number (range `01'\n"
"   through `53'). ISO weeks start with Monday and end with\n"
"   Sunday. Week `01' of a year is the first week which has the\n"
"   majority of its days in that year; this is equivalent to the\n"
"   week containing the year's first Thursday, and it is also\n"
"   equivalent to the week containing January 4. Week `01' of a\n"
"   year can contain days from the previous year. The week\n"
"   before week `01' of a year is the last week (`52' or `53') of\n"
"   the previous year even if it contains days from the new year.\n"
"\n"
"   This format was first standardized by POSIX.2-1992 and by\n"
"   ISO C99.\n"
"\n"
"`%w'\n"
"   The day of the week as a decimal number (range `0' through\n"
"   `6'), Sunday being `0'.\n"
"\n"
"`%W'\n"
"   The week number of the current year as a decimal number\n"
"   (range `00' through `53'), starting with the first Monday as\n"
"   the first day of the first week. All days preceding the\n"
"   first Monday in the year are considered to be in week `00'.\n"
"\n"
"`%x'\n"
"   The preferred date representation for the current locale.\n"
"\n"
"`%X'\n"
"   The preferred time of day representation for the current\n"
"   locale.\n"
"\n"
"`%y'\n"
"   The year without a century as a decimal number (range `00'\n"
"   through `99'). This is equivalent to the year modulo 100.\n"
"\n"
"`%Y'\n"
"   The year as a decimal number, using the Gregorian calendar.\n"
"   Years before the year `1' are numbered `0', `-1', and so on.\n"
"\n"
"`%z'\n"
"   RFC 822/ISO 8601:1988 style numeric time zone (e.g., `-0600'\n"
"   or `+0100'), or nothing if no time zone is determinable.\n"
"\n"
"   This format was first standardized by ISO C99 and by\n"
"   POSIX.1-2001 but was previously available as a GNU extension.\n"
"\n"
"   In the POSIX locale, a full RFC 822 timestamp is generated by\n"
"   the format `\"%a, %d %b %Y %H:%M:%S %z\"' (or the equivalent\n"
"   `\"%a, %d %b %Y %T %z\"').\n"
"\n"
"`%Z'\n"
"   The time zone abbreviation (empty if the time zone can't be\n"
"   determined).\n"
"\n"
"`%%'\n"
"   A literal `%' character.");

  Gtk::ScrolledWindow* time_scwin = MK_SCWIN;
  time_scwin->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  time_scwin->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
  time_scwin->add(*time_helptext);

  Gtk::HBox* time_example_hbox = MK_HBOX;
  time_example_hbox->pack_start(*MK_LABEL("Example:"), false, false, 0);
  time_example_hbox->pack_start(this->time_example, false, false, 0);

  Gtk::VBox* page_timeformat = MK_VBOX;
  page_timeformat->set_border_width(5);
  page_timeformat->pack_start(*time_info_label, false, false, 0);
  page_timeformat->pack_start(*time_info_label2, false, false, 0);
  page_timeformat->pack_start(*time_format_entry, false, false, 0);
  page_timeformat->pack_start(*time_info_label3, false, false, 0);
  page_timeformat->pack_start(*time_short_format_entry, false, false, 0);
  page_timeformat->pack_start(*time_example_hbox, false, false, 0);
  page_timeformat->pack_start(*time_scwin, true, true, 0);

  this->update_time_example();

  /* Button bar. */
  Gtk::HBox* button_bar = MK_HBOX;
  Gtk::Button* close_but = MK_BUT(Gtk::Stock::CLOSE);
  button_bar->pack_end(*close_but, false, false, 0);
  //button_bar->pack_start(*MK_HSEP, true, true, 0);

  /* Notebook packing and window stuff. */
  Gtk::Notebook* notebook = Gtk::manage(new Gtk::Notebook);
  notebook->append_page(*page_misc, "Misc");
  notebook->append_page(*page_notifications, "Notifications");
  notebook->append_page(*page_launch, "Launcher");
  notebook->append_page(*page_timeformat, "Time format");
  notebook->append_page(*page_network, "Network");

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

  close_but->signal_clicked().connect(sigc::mem_fun
      (*this, &GuiConfiguration::close));
  time_format_entry->signal_changed().connect(sigc::mem_fun
      (*this, &GuiConfiguration::update_time_example));
  time_format_entry->signal_activate().connect(sigc::mem_fun
      (*this, &GuiConfiguration::update_time_example));
}

/* ---------------------------------------------------------------- */

void
GuiConfiguration::close (void)
{
  Config::save_to_file();
  this->WinBase::close();
}

/* ---------------------------------------------------------------- */

void
GuiConfiguration::update_time_example (void)
{
  Glib::ustring format = **Config::conf.get_value("evetime.time_format");

  char buffer[128];
  time_t current = ::time(0);
  strftime(buffer, 128, format.c_str(), ::gmtime(&current));
  Glib::ustring example(buffer);

  this->time_example.set_text(buffer);
}
