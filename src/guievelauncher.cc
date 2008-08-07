#include <iostream>

#include <gtkmm/table.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/separator.h>
#include <gtkmm/box.h>
#include <gtkmm/stock.h>
#include <gtkmm/button.h>
#include <gtkmm/image.h>
#include <gtkmm/frame.h>
#include <gtkmm/messagedialog.h>

#include "gtkdefines.h"
#include "exception.h"
#include "helpers.h"
#include "bgprocess.h"
#include "config.h"
#include "guievelauncher.h"

#define EVE_CMD_AMOUNT 5

Gtk::Window*
GuiEveLauncher::launch_eve (void)
{
  std::vector<std::string> commands;
  for (unsigned int i = 0; i < EVE_CMD_AMOUNT; ++i)
  {
    std::string key;
    if (i == 0)
      key = "eve_command";
    else
      key = "eve_command_" + Helpers::get_string_from_uint(i + 1);

    std::string cmd = Config::conf.get_value("settings." + key)->get_string();
    if (!cmd.empty())
      commands.push_back(cmd);
  }

  if (commands.size() == 0)
    throw Exception("There is no EVE command configured.");

  if (commands.size() == 1)
  {
    std::vector<std::string> args = Helpers::tokenize_cmd(commands[0]);
    new BGProcess(args);
    return 0;
  }
  else
  {
    return new GuiEveLauncher(commands);
  }
}

/* ---------------------------------------------------------------- */

GuiEveLauncher::GuiEveLauncher (std::vector<std::string> const& cmds)
{
  Gtk::Table* launch_table = Gtk::manage(new Gtk::Table
      (EVE_CMD_AMOUNT, 3, false));
  launch_table->set_col_spacings(5);
  launch_table->set_row_spacings(1);
  launch_table->set_border_width(5);

  for (unsigned int i = 0; i < cmds.size(); ++i)
  {
    Gtk::Label* eve_cmd_label = MK_LABEL("Command "
        + Helpers::get_string_from_uint(i + 1) + ":");
    Gtk::Entry* eve_cmd_entry = MK_ENTRY;
    eve_cmd_entry->set_text(cmds[i]);
    eve_cmd_entry->set_editable(false);
    Gtk::Button* launch_but = MK_BUT0;
    launch_but->set_image(*MK_IMG(Gtk::Stock::EXECUTE, Gtk::ICON_SIZE_MENU));
    launch_but->signal_clicked().connect(sigc::bind(sigc::mem_fun
        (*this, &GuiEveLauncher::run_command), cmds[i]));

    launch_table->attach(*eve_cmd_label, 0, 1, i, i + 1, Gtk::FILL);
    launch_table->attach(*eve_cmd_entry, 1, 2, i, i + 1, Gtk::EXPAND|Gtk::FILL);
    launch_table->attach(*launch_but, 2, 3, i, i + 1, Gtk::FILL);
  }

  /* Frame. */
  Gtk::Frame* main_frame = MK_FRAME0;
  main_frame->set_shadow_type(Gtk::SHADOW_OUT);
  main_frame->add(*launch_table);

  /* Button bar. */
  Gtk::HBox* button_bar = MK_HBOX;
  Gtk::Button* close_but = MK_BUT(Gtk::Stock::CLOSE);
  button_bar->pack_start(*MK_HSEP, true, true, 0);
  button_bar->pack_end(*close_but, false, false, 0);

  Gtk::VBox* main_box = MK_VBOX;
  main_box->set_border_width(5);
  main_box->pack_start(*main_frame, false, false, 0);
  main_box->pack_end(*button_bar, false, false, 0);

  close_but->signal_clicked().connect(sigc::mem_fun(*this, &WinBase::close));

  this->set_title("EVE Launcher - GtkEveMon");
  this->add(*main_box);
  this->set_default_size(500, 150);
  this->show_all();
}

/* ---------------------------------------------------------------- */

void
GuiEveLauncher::run_command (std::string const& cmd)
{
  std::vector<std::string> args = Helpers::tokenize_cmd(cmd);
  new BGProcess(args);
}
