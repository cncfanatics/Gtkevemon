#include <iostream>
#include <gtkmm/label.h>
#include <gtkmm/table.h>
#include <gtkmm/box.h>
#include <gtkmm/separator.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/image.h>
#include <gtkmm/button.h>
#include <gtkmm/stock.h>
#include <gtkmm/frame.h>
#include <gtkmm/messagedialog.h>

#include "gtkdefines.h"
#include "gtkhelpers.h"
#include "exception.h"
#include "apicharlist.h"
#include "config.h"
#include "guiuserdata.h"

GuiUserData::GuiUserData (void)
  : apply_button(Gtk::Stock::APPLY)
{
  /* Setup the EVE API fetcher. */
  this->charlist_fetcher.set_doctype(EVE_API_DOCTYPE_CHARLIST);
  this->charlist_fetcher.signal_done().connect(sigc::mem_fun
      (*this, &GuiUserData::on_charlist_available));

  /* Create the store for the tree view. */
  this->char_store = Gtk::ListStore::create(this->char_cols);
  this->char_list.set_model(this->char_store);
  this->char_list.append_column_editable(" ", this->char_cols.selected);
  this->char_list.append_column("Name", this->char_cols.name);
  this->char_list.append_column("ID", this->char_cols.charid);
  this->char_list.append_column("Corporation", this->char_cols.corp);

  this->combo_store = Gtk::ListStore::create(this->combo_cols);
  this->history_box.set_model(this->combo_store);
  this->history_box.pack_start(this->combo_cols.userid, true);
  this->history_box.pack_start(this->combo_cols.chars, false);

  /* Build GUI. */
  Gtk::Label* history_label = MK_LABEL("Accounts:");
  Gtk::HBox* history_hbox = MK_HBOX;
  history_hbox->pack_start(*history_label, false, false, 0);
  history_hbox->pack_start(this->history_box, true, true, 0);

  Gtk::HBox* info1_hbox = Gtk::manage(new Gtk::HBox(false, 10));
  info1_hbox->pack_start(*Gtk::manage(new Gtk::Image
      (Gtk::Stock::DIALOG_INFO, Gtk::ICON_SIZE_DIALOG)), false, false, 0);

  Gtk::Label* info1_label = MK_LABEL(
      "First, you need to enter your user id and your LIMITED API key. "
      "This information depends on your account. "
      "You can get the information at:");
  info1_label->set_width_chars(50);
  info1_label->set_line_wrap(true);
  info1_label->set_alignment(Gtk::ALIGN_LEFT);
  Gtk::Label* info1b_label = MK_LABEL("http://myeve.eve-online.com/api");
  info1b_label->set_alignment(Gtk::ALIGN_LEFT);
  info1b_label->set_selectable(true);
  Gtk::VBox* info1_vbox = MK_VBOX0;
  info1_vbox->pack_start(*info1_label, false, false, 0);
  info1_vbox->pack_start(*info1b_label, false, false, 0);
  info1_hbox->pack_start(*info1_vbox, true, true, 0);

  Gtk::Table* data_table = Gtk::manage(new Gtk::Table(2, 2));
  data_table->set_row_spacings(5);
  data_table->set_col_spacings(5);
  Gtk::Label* userid_label = MK_LABEL("User ID:");
  Gtk::Label* apikey_label = MK_LABEL("API Key:");
  userid_label->set_alignment(Gtk::ALIGN_LEFT);
  apikey_label->set_alignment(Gtk::ALIGN_LEFT);
  data_table->attach(*userid_label, 0, 1, 0, 1, Gtk::FILL, Gtk::FILL);
  data_table->attach(*apikey_label, 0, 1, 1, 2, Gtk::FILL, Gtk::FILL);
  data_table->attach(this->userid_entry, 1, 2, 0, 1,
      Gtk::FILL|Gtk::EXPAND, Gtk::FILL);
  data_table->attach(this->apikey_entry, 1, 2, 1, 2,
      Gtk::FILL|Gtk::EXPAND, Gtk::FILL);

  Gtk::HBox* apply_separator = MK_HBOX;
  apply_separator->pack_start(*MK_HSEP, true, true, 0);
  apply_separator->pack_start(this->apply_button, false, false, 0);

  Gtk::HBox* info2_hbox = Gtk::manage(new Gtk::HBox(false, 10));
  info2_hbox->pack_start(*Gtk::manage(new Gtk::Image
      (Gtk::Stock::DIALOG_INFO, Gtk::ICON_SIZE_DIALOG)), false, false, 0);
  Gtk::Label* info2_label = MK_LABEL(
      "If you entered your user information, click the button above "
      "to load the character list. Select the characters for which you "
      "want observation through GtkEveMon.");
  info2_label->set_alignment(Gtk::ALIGN_LEFT);
  info2_label->set_line_wrap(true);
  info2_hbox->pack_start(*info2_label, true, true, 0);

  Gtk::ScrolledWindow* scwin = Gtk::manage(new Gtk::ScrolledWindow);
  scwin->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
  scwin->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  scwin->add(this->char_list);

  Gtk::VBox* settings_vbox = MK_VBOX;
  settings_vbox->set_border_width(5);
  settings_vbox->pack_start(*history_hbox, false, false, 0);
  settings_vbox->pack_start(*MK_HSEP, false, false, 0);
  settings_vbox->pack_start(*info1_hbox, false, false, 0);
  settings_vbox->pack_start(*data_table, false, false, 0);
  settings_vbox->pack_start(*apply_separator, false, false, 0);
  settings_vbox->pack_start(*info2_hbox, false, false, 0);
  settings_vbox->pack_start(*scwin, true, true, 0);

  Gtk::Frame* main_frame = MK_FRAME0;
  main_frame->add(*settings_vbox);
  main_frame->set_shadow_type(Gtk::SHADOW_OUT);

  Gtk::Button* but_close = MK_BUT(Gtk::Stock::CLOSE);
  Gtk::Button* but_add = MK_BUT(Gtk::Stock::ADD);

  Gtk::HBox* button_bar = MK_HBOX;
  button_bar->pack_start(*but_close, false, false, 0);
  button_bar->pack_start(*MK_HSEP, true, true, 0);
  button_bar->pack_start(*but_add, false, false, 0);

  Gtk::VBox* main_vbox = MK_VBOX;
  main_vbox->set_border_width(5);
  main_vbox->pack_start(*main_frame, true, true, 0);
  main_vbox->pack_end(*button_bar, false, false, 0);

  this->add(*main_vbox);
  this->set_title("User Data - GtkEveMon");
  this->show_all();
  this->set_size_request(-1, 450);

  this->init_from_config();

  this->apply_button.signal_clicked().connect(sigc::mem_fun
      (*this, &GuiUserData::on_apply_clicked));
  but_close->signal_clicked().connect(sigc::mem_fun(*this, &WinBase::close));
  but_add->signal_clicked().connect(sigc::mem_fun
      (*this, &GuiUserData::on_add_clicked));
  this->history_box.signal_changed().connect(sigc::mem_fun
      (*this, &GuiUserData::on_history_selected));
}

/* ---------------------------------------------------------------- */

void
GuiUserData::init_from_config (void)
{
  /* Populate history. */
  this->combo_store->clear();
  ConfSectionPtr accounts = Config::conf.get_section("accounts");
  conf_sections_t::iterator iter = accounts->sections_begin();
  unsigned int acc_amount = 0;
  while (iter != accounts->sections_end())
  {
    Gtk::TreeModel::Row row = *this->combo_store->append();
    row[this->combo_cols.userid] = iter->first;
    row[this->combo_cols.chars] = **iter->second->get_value("chars");
    iter++;
    acc_amount += 1;
  }

  if (acc_amount == 0)
    this->history_box.set_sensitive(false);
}

/* ---------------------------------------------------------------- */

void
GuiUserData::on_charlist_available (EveApiData data)
{
  this->apply_button.set_sensitive(true);

  if (data.data.get() == 0)
  {
    this->print_error(data.exception);
    return;
  }

  ApiCharacterListPtr clist = ApiCharacterList::create();
  try
  {
    clist->set_api_data(data);
    if (clist->is_locally_cached())
      this->print_error(data.exception, true);
  }
  catch (Exception& e)
  {
    this->print_error(e);
    return;
  }

  /* Successfully requested character list. */
  ApiCharList const& chars = clist->chars;
  this->char_store->clear();
  std::string char_string;

  if (chars.empty())
  {
    this->print_error("No characters found for that account!");
    return;
  }

  //std::cout << "Character list has " << chars.size()
  //    << " entries:" << std::endl;
  for (unsigned int i = 0; i < chars.size(); ++i)
  {
    //std::cout << i << ". Character: " << chars[i].name << " ["
    //    << chars[i].char_id << "]" << ", Corp: "
    //    << chars[i].corp << std::endl;

    Gtk::TreeModel::Row row = *this->char_store->append();
    row[this->char_cols.selected] = false;
    row[this->char_cols.name] = chars[i].name;
    row[this->char_cols.charid] = chars[i].char_id;
    row[this->char_cols.corp] = chars[i].corp;

    if (i != 0)
      char_string += ", ";
    char_string += chars[i].name;
  }

  /* Insert the data to the history and the combo box. */
  ConfSectionPtr sect = Config::conf.get_or_create_section
      ("accounts." + this->userid_entry.get_text());
  sect->add("apikey", ConfValue::create(this->apikey_entry.get_text()));
  sect->add("chars", ConfValue::create(char_string));
  Config::save_to_file();
}

/* ---------------------------------------------------------------- */

void
GuiUserData::on_apply_clicked (void)
{
  EveApiAuth auth(this->userid_entry.get_text(), this->apikey_entry.get_text());
  this->charlist_fetcher.set_auth(auth);
  this->charlist_fetcher.async_request();
  this->apply_button.set_sensitive(false);
}

/* ---------------------------------------------------------------- */

void
GuiUserData::print_error (std::string const& error, bool cached)
{
  Gtk::MessageType message_type;
  Glib::ustring message;
  if (cached)
  {
    message = "Using cached version of the character list!";
    message_type = Gtk::MESSAGE_WARNING;
  }
  else
  {
    message = "Error retrieving character list!";
    message_type = Gtk::MESSAGE_ERROR;
  }

  Gtk::MessageDialog md(*this, message, false, message_type, Gtk::BUTTONS_OK);
  md.set_secondary_text("There was an error while requesting the character "
      "list from the EVE API. The EVE API is either offline, or the "
      "requested document is not understood by GtkEveMon. "
      "The error message is:\n\n" + GtkHelpers::locale_to_utf8(error));
  md.set_title("Error - GtkEveMon");
  md.run();
}

/* ---------------------------------------------------------------- */

void
GuiUserData::on_add_clicked (void)
{
  std::string user_id = this->userid_entry.get_text();
  std::string api_key = this->apikey_entry.get_text();

  EveApiAuth auth(user_id, api_key);

  Gtk::TreeModel::Children childs = this->char_store->children();
  for (Gtk::TreeIter iter = childs.begin(); iter != childs.end(); iter++)
  {
    bool selected = (*iter)[this->char_cols.selected];
    if (!selected)
      continue;

    Glib::ustring charid = (*iter)[this->char_cols.charid];
    auth.char_id = charid;
    this->sig_char_added.emit(auth);
  }

  this->close();
}

/* ---------------------------------------------------------------- */

void
GuiUserData::on_history_selected (void)
{
  Gtk::TreeModel::iterator iter = this->history_box.get_active();
  if (iter)
  {
    Gtk::TreeModel::Row row = *iter;
    Glib::ustring userid = row[this->combo_cols.userid];
    ConfSectionPtr sect = Config::conf.get_section
        ("accounts." + userid);
    std::string apikey = **sect->get_value("apikey");

    this->userid_entry.set_text(userid);
    this->apikey_entry.set_text(apikey);
    this->char_store->clear();
  }
}

/* ---------------------------------------------------------------- */

sigc::signal<void, EveApiAuth>
GuiUserData::signal_char_added (void)
{
  return this->sig_char_added;
}
