#include <iostream>
#include <sstream>
#include <gtkmm/main.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/separator.h>
#include <gtkmm/table.h>
#include <gtkmm/box.h>
#include <gtkmm/treeview.h>
#include <gtkmm/image.h>
#include <gtkmm/stock.h>
#include <gtkmm/scrolledwindow.h>

#include "config.h"
#include "evetime.h"
#include "helpers.h"
#include "exception.h"
#include "apiintraining.h"
#include "apicharsheet.h"
#include "apiskilltree.h"
#include "imagestore.h"
#include "gtkdefines.h"
#include "guiskill.h"
#include "guiskillplanner.h"
#include "guixmlsource.h"
#include "gtkcharpage.h"

GtkCharPage::GtkCharPage (void)
  : Gtk::VBox(false, 5),
    info_display(INFO_STYLE_TOP_HSEP)
{
  /* Setup the API HTTP fetchers and API data. */
  this->sheet_fetcher.set_doctype(EVE_API_DOCTYPE_CHARSHEET);
  this->training_fetcher.set_doctype(EVE_API_DOCTYPE_INTRAINING);
  this->sheet = ApiCharSheet::create();
  this->training = ApiInTraining::create();

  /* GUI stuff. */
  Gtk::TreeViewColumn* name_column = Gtk::manage(new Gtk::TreeViewColumn);
  name_column->set_title("Skill name (rank)");
  name_column->pack_start(this->skill_cols.icon, false);
  Gtk::CellRendererText* name_renderer = Gtk::manage(new Gtk::CellRendererText);
  name_column->pack_start(*name_renderer, true);
  name_column->add_attribute(name_renderer->property_markup(),
      this->skill_cols.name);

  this->skill_store = Gtk::TreeStore::create(this->skill_cols);
  this->skill_store->set_sort_column
      (this->skill_cols.name, Gtk::SORT_ASCENDING);
  this->skill_view.set_model(this->skill_store);
  this->skill_view.set_rules_hint(true);
  this->skill_view.append_column(*name_column);
  this->skill_view.append_column("Points", this->skill_cols.points);
  this->skill_view.append_column("Level", this->skill_cols.level);
  this->skill_view.get_column(0)->set_expand(true);
  this->skill_view.get_column(1)->get_first_cell_renderer
      ()->property_xalign() = 1.0f;

  this->char_image.property_xalign() = 1.0f;
  this->refresh_but.set_image(*Gtk::manage(new Gtk::Image
      (Gtk::Stock::REFRESH, Gtk::ICON_SIZE_BUTTON)));
  this->info_but.set_image(*Gtk::manage(new Gtk::Image
      (Gtk::Stock::INFO, Gtk::ICON_SIZE_BUTTON)));
  this->refresh_but.set_relief(Gtk::RELIEF_NONE);
  this->info_but.set_relief(Gtk::RELIEF_NONE);

  this->char_name_label.property_xalign() = 0.0f;
  this->char_info_label.property_xalign() = 0.0f;
  this->corp_label.property_xalign() = 0.0f;
  this->balance_label.property_xalign() = 0.0f;
  this->skill_points_label.property_xalign() = 0.0f;
  this->known_skills_label.property_xalign() = 0.0f;
  this->attr_cha_label.property_xalign() = 0.0f;
  this->attr_int_label.property_xalign() = 0.0f;
  this->attr_per_label.property_xalign() = 0.0f;
  this->attr_mem_label.property_xalign() = 0.0f;
  this->attr_wil_label.property_xalign() = 0.0f;
  this->training_label.property_xalign() = 0.0f;
  this->remaining_label.property_xalign() = 0.0f;
  this->finish_eve_label.property_xalign() = 0.0f;
  this->finish_local_label.property_xalign() = 0.0f;
  this->spph_label.property_xalign() = 1.0f;
  this->live_sp_label.property_xalign() = 1.0f;

  Gtk::ScrolledWindow* scwin = Gtk::manage(new Gtk::ScrolledWindow);
  scwin->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  scwin->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
  scwin->add(this->skill_view);
  scwin->show_all();

  Gtk::Table* info_table = Gtk::manage(new Gtk::Table(5, 7));
  info_table->set_col_spacings(10);
  Gtk::Label* corp_desc = MK_LABEL("Corporation:");
  Gtk::Label* isk_desc = MK_LABEL("Balance:");
  Gtk::Label* skillpoints_desc = MK_LABEL("Skill points:");
  Gtk::Label* knownskills_desc = MK_LABEL("Known skills:");
  Gtk::Label* attr_charisma_desc = MK_LABEL("Charisma:");
  Gtk::Label* attr_intelligence_desc = MK_LABEL("Intelligence:");
  Gtk::Label* attr_perception_desc = MK_LABEL("Perception:");
  Gtk::Label* attr_memory_desc = MK_LABEL("Memory:");
  Gtk::Label* attr_willpower_desc = MK_LABEL("Willpower:");
  corp_desc->property_xalign() = 0.0f;
  isk_desc->property_xalign() = 0.0f;
  skillpoints_desc->property_xalign() = 0.0f;
  knownskills_desc->property_xalign() = 0.0f;
  attr_charisma_desc->property_xalign() = 0.0f;
  attr_intelligence_desc->property_xalign() = 0.0f;
  attr_perception_desc->property_xalign() = 0.0f;
  attr_memory_desc->property_xalign() = 0.0f;
  attr_willpower_desc->property_xalign() = 0.0f;

  Gtk::Button* close_but = MK_BUT0;
  close_but->set_relief(Gtk::RELIEF_NONE);
  close_but->set_image(*Gtk::manage(new Gtk::Image
      (Gtk::Stock::CLOSE, Gtk::ICON_SIZE_MENU)));
  Gtk::HBox* close_but_hbox = MK_HBOX;
  close_but_hbox->pack_end(*close_but, false, false, 0);
  Gtk::VBox* close_but_vbox = MK_VBOX;
  close_but_vbox->pack_start(*close_but_hbox, false, false, 0);

  info_table->attach(this->char_image, 0, 1, 0, 5, Gtk::FILL, Gtk::FILL);
  info_table->attach(this->char_name_label, 1, 2, 0, 1, Gtk::FILL, Gtk::FILL);
  info_table->attach(*corp_desc, 1, 2, 1, 2, Gtk::FILL, Gtk::FILL);
  info_table->attach(*isk_desc, 1, 2, 2, 3, Gtk::FILL, Gtk::FILL);
  info_table->attach(*skillpoints_desc, 1, 2, 3, 4, Gtk::FILL, Gtk::FILL);
  info_table->attach(*knownskills_desc, 1, 2, 4, 5, Gtk::FILL, Gtk::FILL);
  info_table->attach(this->char_info_label, 2, 3, 0, 1, Gtk::FILL, Gtk::FILL);
  info_table->attach(this->corp_label, 2, 3, 1, 2, Gtk::FILL, Gtk::FILL);
  info_table->attach(this->balance_label, 2, 3, 2, 3, Gtk::FILL, Gtk::FILL);
  info_table->attach(this->skill_points_label, 2, 3, 3, 4,
      Gtk::FILL, Gtk::FILL);
  info_table->attach(this->known_skills_label, 2, 3, 4, 5,
      Gtk::FILL, Gtk::FILL);
  info_table->attach(*MK_VSEP, 3, 4, 0, 5, Gtk::FILL, Gtk::FILL);
  info_table->attach(*attr_charisma_desc, 4, 5, 0, 1, Gtk::FILL, Gtk::FILL);
  info_table->attach(*attr_intelligence_desc, 4, 5, 1, 2, Gtk::FILL, Gtk::FILL);
  info_table->attach(*attr_perception_desc, 4, 5, 2, 3, Gtk::FILL, Gtk::FILL);
  info_table->attach(*attr_memory_desc, 4, 5, 3, 4, Gtk::FILL, Gtk::FILL);
  info_table->attach(*attr_willpower_desc, 4, 5, 4, 5, Gtk::FILL, Gtk::FILL);
  info_table->attach(this->attr_cha_label, 5, 6, 0, 1, Gtk::FILL, Gtk::FILL);
  info_table->attach(this->attr_int_label, 5, 6, 1, 2, Gtk::FILL, Gtk::FILL);
  info_table->attach(this->attr_per_label, 5, 6, 2, 3, Gtk::FILL, Gtk::FILL);
  info_table->attach(this->attr_mem_label, 5, 6, 3, 4, Gtk::FILL, Gtk::FILL);
  info_table->attach(this->attr_wil_label, 5, 6, 4, 5, Gtk::FILL, Gtk::FILL);
  info_table->attach(*close_but_vbox, 6, 7, 0, 2,
      Gtk::FILL | Gtk::EXPAND, Gtk::SHRINK | Gtk::FILL);

  Gtk::Table* train_table = Gtk::manage(new Gtk::Table(4, 3));
  train_table->set_col_spacings(10);
  Gtk::Label* train_desc = MK_LABEL("<b>Training:</b>");
  Gtk::Label* remain_desc = MK_LABEL("Remaining:");
  Gtk::Label* finish_eve_desc = MK_LABEL("Finish (EVE):");
  Gtk::Label* finish_local_desc = MK_LABEL("Finish (local):");
  train_desc->set_use_markup(true);
  train_desc->property_xalign() = 0.0f;
  remain_desc->property_xalign() = 0.0f;
  finish_eve_desc->property_xalign() = 0.0f;
  finish_local_desc->property_xalign() = 0.0f;
  train_table->attach(*train_desc, 0, 1, 0, 1, Gtk::FILL, Gtk::FILL);
  train_table->attach(*remain_desc, 0, 1, 1, 2, Gtk::FILL, Gtk::FILL);
  train_table->attach(*finish_eve_desc, 0, 1, 2, 3, Gtk::FILL, Gtk::FILL);
  train_table->attach(*finish_local_desc, 0, 1, 3, 4, Gtk::FILL, Gtk::FILL);
  train_table->attach(this->training_label, 1, 2, 0, 1);
  train_table->attach(this->remaining_label, 1, 2, 1, 2);
  train_table->attach(this->finish_eve_label, 1, 2, 2, 3);
  train_table->attach(this->finish_local_label, 1, 2, 3, 4);
  Gtk::HBox* mini_but_box = MK_HBOX0;
  mini_but_box->pack_end(this->refresh_but, false, false, 0);
  mini_but_box->pack_end(this->info_but, false, false, 0);
  train_table->attach(*mini_but_box, 2, 3, 0, 2, Gtk::FILL, Gtk::SHRINK);
  train_table->attach(this->spph_label, 2, 3, 2, 3, Gtk::FILL, Gtk::SHRINK);
  train_table->attach(this->live_sp_label, 2, 3, 3, 4, Gtk::FILL, Gtk::SHRINK);

  this->set_border_width(5);
  this->pack_start(*info_table, false, false, 0);
  this->pack_start(*scwin, true, true, 0);
  this->pack_start(*train_table, false, false, 0);
  this->pack_start(this->info_display, false, false, 0);

  /* Setup tooltips. */
  this->tooltips.set_tip(*close_but, "Closes the character");
  this->tooltips.set_tip(this->info_but, "Infomation about cached sheets");
  this->tooltips.set_tip(this->refresh_but, "Request character sheets");

  /* Signals. */
  close_but->signal_clicked().connect(sigc::mem_fun
      (*this, &GtkCharPage::on_close_clicked));
  this->refresh_but.signal_clicked().connect(sigc::mem_fun
      (*this, &GtkCharPage::request_documents));
  this->info_but.signal_clicked().connect(sigc::mem_fun
      (*this, &GtkCharPage::on_info_clicked));
  this->skill_view.signal_row_activated().connect(sigc::mem_fun
      (*this, &GtkCharPage::on_skill_activated));
  this->skill_view.set_has_tooltip(true);
  this->skill_view.signal_query_tooltip().connect(sigc::mem_fun
      (*this, &GtkCharPage::on_query_skillview_tooltip));

  this->sheet_fetcher.signal_done().connect(sigc::mem_fun
      (*this, &GtkCharPage::on_charsheet_available));
  this->training_fetcher.signal_done().connect(sigc::mem_fun
      (*this, &GtkCharPage::on_intraining_available));

  Glib::signal_timeout().connect(sigc::mem_fun(*this,
      &GtkCharPage::update_remaining), CHARPAGE_REMAINING_UPDATE);
  Glib::signal_timeout().connect(sigc::mem_fun(*this,
      &GtkCharPage::on_live_sp_value_update), CHARPAGE_LIVE_SP_LABEL_UPDATE);
  Glib::signal_timeout().connect(sigc::mem_fun(*this,
      &GtkCharPage::on_live_sp_image_update), CHARPAGE_LIVE_SP_IMAGE_UPDATE);
  Glib::signal_timeout().connect(sigc::mem_fun(*this,
      &GtkCharPage::check_expired_sheets), CHARPAGE_CHECK_EXPIRED_SHEETS);

  /* Update GUI. */
  this->update_charsheet_details();
  this->update_training_details();

  this->show_all();
  this->info_display.hide();
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::set_character (EveApiAuth const& character)
{
  this->character = character;
  this->sheet_fetcher.set_auth(character);
  this->training_fetcher.set_auth(character);
  this->char_image.set(this->character.char_id);
  this->update_charsheet_details();
  this->request_documents();
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::update_charsheet_details (void)
{
  /* Set character information. */
  if (!this->sheet->valid)
  {
    this->char_name_label.set_text("<b>" + this->character.char_id + "</b>");
    this->char_name_label.set_use_markup(true);
    this->char_info_label.set_text("---");
    this->corp_label.set_text("---");
    this->balance_label.set_text("---");
    this->skill_points_label.set_text("---");
    this->known_skills_label.set_text("---");
    this->attr_cha_label.set_text("---");
    this->attr_int_label.set_text("---");
    this->attr_per_label.set_text("---");
    this->attr_mem_label.set_text("---");
    this->attr_wil_label.set_text("---");
  }
  else
  {
    this->char_name_label.set_text("<b>" + this->sheet->name + "</b>");
    this->char_name_label.set_use_markup(true);
    this->char_info_label.set_text(this->sheet->gender + ", "
        + this->sheet->race + ", " + this->sheet->bloodline);
    this->corp_label.set_text(this->sheet->corp);
    this->balance_label.set_text(Helpers::get_dotted_isk
        (this->sheet->balance) + " ISK");

    this->skill_points_label.set_text(Helpers::get_dotted_str_from_uint
        (this->skill_info.total_sp));
    this->known_skills_label.set_text(Helpers::get_string_from_uint
        (this->sheet->skills.size()) + " known skills ("
        + Helpers::get_string_from_uint(this->skill_info.skills_at_five)
        + " at V)");

    this->attr_cha_label.set_text(Helpers::get_string_from_double
        (this->sheet->total_cha, 2));
    this->attr_int_label.set_text(Helpers::get_string_from_double
        (this->sheet->total_int, 2));
    this->attr_per_label.set_text(Helpers::get_string_from_double
        (this->sheet->total_per, 2));
    this->attr_mem_label.set_text(Helpers::get_string_from_double
        (this->sheet->total_mem, 2));
    this->attr_wil_label.set_text(Helpers::get_string_from_double
        (this->sheet->total_wil, 2));
  }

  this->update_skill_list();
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::update_training_details (void)
{
  /* Set training information. */
  if (!this->training->valid)
  {
    this->training_label.set_text("---");
    this->remaining_label.set_text("---");
    this->finish_eve_label.set_text("---");
    this->finish_local_label.set_text("---");
    this->spph_label.set_text("---");
    this->live_sp_label.set_text("---");
  }
  else
  {
    if (this->training->in_training)
    {
      this->training_label.set_text(this->get_skill_in_training());
      this->finish_eve_label.set_text
          (EveTime::get_gm_time_string(this->training->end_time_t));
      this->finish_local_label.set_text(EveTime::get_local_time_string
          (EveTime::adjust_local_time(this->training->end_time_t)));
    }
    else
    {
      this->training_label.set_text("No skill in training!");
      this->remaining_label.set_text("---");
      this->finish_eve_label.set_text("---");
      this->finish_local_label.set_text("---");
      this->spph_label.set_text("0 SP per hour");
      this->live_sp_label.set_text("---");
    }
  }
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::update_skill_list (void)
{
  this->skill_store->clear();

  if (!this->sheet->valid)
    return;

  ApiSkillTreePtr tree;

  try
  {
    tree = ApiSkillTree::request();
  }
  catch (Exception& e)
  {
    this->on_skilltree_error(e);
    return;
  }

  /* Cache skill in training. */
  ApiSkill skill_training;
  skill_training.id = -1;
  skill_training.group = -1;
  if (this->training->valid && this->training->in_training)
    skill_training = *this->skill_info.char_skill->details;

  /* Append all groups to the store. Save their iterators for the children.
   * Format is <group_id, <model iter, group sp> >. */
  typedef std::map<int, std::pair<Gtk::TreeModel::iterator, int> > IterMapType;
  IterMapType iter_map;
  for (ApiSkillGroupMap::iterator iter = tree->groups.begin();
      iter != tree->groups.end(); iter++)
  {
    std::string name = iter->second.name;
    if (skill_training.group == iter->first)
      name += "  <i>(1 in training)</i>";

    Gtk::TreeModel::iterator siter = this->skill_store->append();
    (*siter)[this->skill_cols.id] = -1;
    (*siter)[this->skill_cols.skill] = 0;
    (*siter)[this->skill_cols.name] = name;
    (*siter)[this->skill_cols.icon] = ImageStore::skillicons[0];
    iter_map.insert(std::make_pair(iter->first, std::make_pair(siter, 0)));
  }

  /* Append all skills to the skill groups. */
  std::vector<ApiCharSheetSkill>& skills = this->sheet->skills;
  for (unsigned int i = 0; i < skills.size(); ++i)
  {
    /* Get skill object. */
    ApiSkill const& skill = *skills[i].details;

    /* Lookup skill group. */
    IterMapType::iterator iiter = iter_map.find(skill.group);
    if (iiter == iter_map.end())
    {
      std::cout << "Error appending skill, unknown group!" << std::endl;
      continue;
    }

    /* Append a new row. */
    Gtk::TreeModel::iterator iter = this->skill_store->append
        (iiter->second.first->children());

    /* Get skill name and set icon. */
    std::string skill_name = skill.name + " ("
        + Helpers::get_string_from_int(skill.rank) + ")";

    if (skill.id == skill_training.id)
    {
      skill_name += "  <i>(in training)</i>";
      (*iter)[this->skill_cols.icon] = ImageStore::skillicons[2];

      /* Update of the SkillInTrainingInfo. */
      this->skill_info.tree_skill_iter = iter;
    }
    else
    {
      if (skills[i].points != skills[i].points_start)
        (*iter)[this->skill_cols.icon] = ImageStore::skillicons[4];
      else if (skills[i].level < 5)
        (*iter)[this->skill_cols.icon] = ImageStore::skillicons[1];
      else
        (*iter)[this->skill_cols.icon] = ImageStore::skillicons[3];
    }

    /* Set skill id, points and name. */
    (*iter)[this->skill_cols.id] = skill.id;
    (*iter)[this->skill_cols.skill] = &skills[i];
    (*iter)[this->skill_cols.points]
        = Helpers::get_dotted_str_from_int(skills[i].points);
    (*iter)[this->skill_cols.name] = skill_name;
    (*iter)[this->skill_cols.level] = ImageStore::skill_progress
        (skills[i].level, skills[i].completed);

    iiter->second.second += skills[i].points;
  }

  /* Update the skillpoints for the groups. */
  for (IterMapType::iterator iter = iter_map.begin();
      iter != iter_map.end(); iter++)
  {
    if (iter->second.second == 0)
    {
      this->skill_store->erase(iter->second.first);
      continue;
    }

    (*iter->second.first)[this->skill_cols.points]
        = Helpers::get_dotted_str_from_int(iter->second.second);

    /* Update of the SkillInTrainingInfo. */
    if (iter->first == skill_training.group)
      this->skill_info.tree_group_iter = iter->second.first;
  }
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::request_documents (void)
{
  bool update_char = true;
  bool update_training = true;

  time_t evetime = EveTime::get_eve_time();
  std::string char_cached("<unknown>");
  std::string train_cached("<unknown>");

  /* Check which docs to re-request. */
  if (this->sheet->valid)
  {
    time_t char_cached_t = this->sheet->get_cached_until_t();
    char_cached = EveTime::get_gm_time_string(char_cached_t);
    if (evetime < char_cached_t)
      update_char = false;
  }

  if (this->training->valid)
  {
    time_t train_cached_t = this->training->get_cached_until_t();
    train_cached = EveTime::get_gm_time_string(train_cached_t);
    if (evetime < train_cached_t)
      update_training = false;
  }

  /* Message user if both docs are up-to-date. */
  if (!update_char && !update_training)
  {
    Gtk::MessageDialog md("Data is already up-to-date. Continue anyway?",
        false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
    md.set_secondary_text("The data you are about to refresh is already "
        "up-to-date.\n\n"
        "Character sheet expires at " + char_cached + "\n"
        "Training sheet expires at " + train_cached + "\n\n"
        "You can continue and rerequest the data, but it will most "
        "likely don't change a thing.");
    md.set_title("Cache Status - GtkEveMon");
    md.set_transient_for(*this->parent_window);
    int result = md.run();

    switch (result)
    {
      default:
      case Gtk::RESPONSE_DELETE_EVENT:
      case Gtk::RESPONSE_NO:
        return;
      case Gtk::RESPONSE_YES:
        update_char = true;
        update_training = true;
        break;
    };
  }

  /* Request the documents. */
  if (update_char && !this->sheet_fetcher.is_busy())
  {
    this->sheet_fetcher.async_request();
  }

  if (update_training && !this->training_fetcher.is_busy())
  {
    this->training_label.set_text("Requesting...");
    this->training_fetcher.async_request();
  }
}

/* ---------------------------------------------------------------- */

bool
GtkCharPage::check_expired_sheets (void)
{
  ConfValuePtr value = Config::conf.get_value("settings.auto_update_sheets");
  if (!value->get_bool())
    return true;

  //std::cout << "Checking for expired sheets..." << std::endl;

  time_t evetime = EveTime::get_eve_time();

  /* Check which docs to re-request. */
  if (!this->training->valid || evetime >= this->training->get_cached_until_t()
      || !this->sheet->valid || evetime >= this->sheet->get_cached_until_t())
    this->request_documents();

  return true;
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::on_charsheet_available (AsyncHttpData data)
{
  if (data.data.get() == 0)
  {
    this->on_charsheet_error(data.exception);
    return;
  }

  try
  {
    this->sheet->set_from_xml(data.data);
  }
  catch (Exception& e)
  {
    this->on_charsheet_error(e);
    return;
  }

  /* Update the GUI. */
  this->api_info_changed();
  this->update_charsheet_details();

  this->on_live_sp_value_update();
  this->on_live_sp_image_update();
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::on_intraining_available (AsyncHttpData data)
{
  if (data.data.get() == 0)
  {
    this->on_intraining_error(data.exception);
    return;
  }

  try
  {
    this->training->set_from_xml(data.data);
  }
  catch (Exception& e)
  {
    this->on_intraining_error(e);
    return;
  }

  /* Update the GUI. */
  this->api_info_changed();
  this->update_charsheet_details();
  this->update_training_details();

  this->on_live_sp_value_update();
  this->on_live_sp_image_update();
  this->update_remaining();
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::api_info_changed (void)
{
  if (this->sheet->valid)
  {
    /* Update worked-up information. */
    this->skill_info.total_sp = 0;
    this->skill_info.skills_at_five = 0;

    for (unsigned int i = 0; i < this->sheet->skills.size(); ++i)
    {
      ApiCharSheetSkill& skill = this->sheet->skills[i];
      this->skill_info.total_sp += skill.points;
      this->skill_info.skills_at_five += (skill.level == 5 ? 1 : 0);
    }

    this->sig_sheet_updated.emit(this->character);
  }

  /* Update worked-up information. */
  if (this->sheet->valid
    && this->training->valid
    && this->training->in_training)
  {
    /* Cache current skill in training and the SP/h. */
    this->skill_info.char_skill = this->sheet->get_skill_for_id
        (this->training->skill);
    this->skill_info.sp_per_hour = this->get_spph_in_training();
    this->spph_label.set_text(Helpers::get_string_from_uint
        ((unsigned int)this->skill_info.sp_per_hour) + " SP per hour");

    /* Update the skill: level, start and dest SP if the character
     * skill information is outdated outdated or the character
     * sheet is still cached and has old info. Create the skill if the
     * character does not even have it. */
    if (this->skill_info.char_skill == 0)
    {
      /* The character does not even have the skill. Create it. */
      ApiSkillTreePtr tree = ApiSkillTree::request();
      ApiSkill const* skill = tree->get_skill_for_id(this->training->skill);

      ApiCharSheetSkill cskill;
      cskill.id = skill->id;
      cskill.level = this->training->to_level - 1;
      cskill.points = 0;
      cskill.points_start = ApiCharSheet::calc_start_sp
          (cskill.level, skill->rank);
      cskill.points_dest = ApiCharSheet::calc_dest_sp
          (cskill.level, skill->rank);
      cskill.completed = 0.0;
      cskill.details = skill;

      this->sheet->skills.push_back(cskill);
      this->skill_info.char_skill = &this->sheet->skills.back();
    }
    else if (this->skill_info.char_skill->level != this->training->to_level - 1)
    {
      /* The character has outdated skill information. Update. */
      ApiCharSheetSkill* skill = this->skill_info.char_skill;
      skill->level = this->training->to_level - 1;
      skill->points_start = ApiCharSheet::calc_start_sp
          (skill->level, skill->details->rank);
      skill->points_dest = ApiCharSheet::calc_dest_sp
          (skill->level, skill->details->rank);
    }

    /* Cache the total skill points for the skill in training. */
    ApiCharSheetSkill* training_skill = this->skill_info.char_skill;
    int training_skill_group = training_skill->details->group;
    this->skill_info.skill_group_sp = 0;
    for (unsigned int i = 0; i < this->sheet->skills.size(); ++i)
    {
      ApiCharSheetSkill& skill = this->sheet->skills[i];
      if (skill.details->group == training_skill_group)
        this->skill_info.skill_group_sp += skill.points;
    }

    /* Since there is a skill in training, remove the points
     * for partial training processes for easier GUI updates
     * for the live SP counting. */
    if (training_skill->points > training_skill->points_start)
    {
      int diff = training_skill->points - training_skill->points_start;
      this->skill_info.total_sp -= diff;
      this->skill_info.skill_group_sp -= diff;
    }
  }
  else
  {
    this->skill_info.char_skill = 0;
    this->skill_info.sp_per_hour = 0;
    this->skill_info.skill_group_sp = 0;
    this->spph_label.set_text("---");
  }

  //std::cout << "Total SP are: " << this->skill_info.total_sp
  //    << " Skill: " << this->skill_info.char_skill->points
  //    << " Dest: " << this->skill_info.char_skill->points_dest << std::endl;
}

/* ---------------------------------------------------------------- */

bool
GtkCharPage::update_remaining (void)
{
  if (this->training->valid && this->training->in_training)
  {
    time_t evetime = EveTime::get_eve_time();
    time_t finish = this->training->end_time_t;
    time_t diff = finish - evetime;

    if (diff < 0)
      this->on_skill_completed();
    else
      this->remaining_label.set_text(this->get_skill_remaining());
  }

  return true;
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::create_tray_notify (void)
{
  this->tray_notify = Gtk::StatusIcon::create(ImageStore::skill);
  this->tray_notify->signal_activate().connect(sigc::mem_fun
     (*this, &GtkCharPage::remove_tray_notify));
  //this->tray_notify->set_blinking(true);
  this->tray_notify->set_tooltip(this->get_char_name() + " has "
      "completed " + this->training_label.get_text() + "!");
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::remove_tray_notify (void)
{
  //this->tray_notify.reset(); // Compile errors for glibmm < 2.16
  this->tray_notify.clear(); // Deprecated
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::on_skill_completed (void)
{
  /* Set up some GUI elements. */
  this->remaining_label.set_text("Completed!");
  this->finish_eve_label.set_text("---");
  this->finish_local_label.set_text("---");
  this->spph_label.set_text("0 SP per hour");
  this->live_sp_label.set_text("---");

  /* Remove skill from training. */
  this->training->in_training = false;

  /* Update skill for the character. */
  ApiCharSheetSkill* cskill = this->skill_info.char_skill;
  if (cskill != 0)
  {
    cskill->level = this->training->to_level;
    cskill->points_start = ApiCharSheet::calc_start_sp
        (cskill->level, cskill->details->rank);
    cskill->points_dest = ApiCharSheet::calc_dest_sp
        (cskill->level, cskill->details->rank);
    cskill->points = cskill->points_start;
    cskill->completed = 0.0;
  }

  /* Update GUI to reflect changes. */
  this->api_info_changed();
  this->update_charsheet_details();

  /* Now bring up some notifications. */
  ConfValuePtr show_popup = Config::conf.get_value
      ("notifications.show_popup_dialog");
  ConfValuePtr show_tray = Config::conf.get_value
      ("notifications.show_tray_icon");
  ConfValuePtr show_info = Config::conf.get_value
      ("notifications.show_info_bar");

  if (show_tray->get_bool())
    this->create_tray_notify();

  if (show_info->get_bool())
    this->info_display.append(INFO_NOTIFICATION, "Skill training for <b>"
        + this->training_label.get_text() + "</b> completed!");

  if (show_popup->get_bool())
  {
    Gtk::MessageDialog md("Skill training completed!",
        false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
    md.set_secondary_text("Congratulations. <b>" + this->get_char_name()
        + "</b> has just completed the skill training for <b>"
        + this->training_label.get_text() + "</b>.", true);
    md.set_title("Skill training completed!");
    md.set_transient_for(*this->parent_window);
    md.run();

    this->remove_tray_notify();
  }
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::on_close_clicked (void)
{
  this->sig_close_request.emit(this->character);
}

/* ---------------------------------------------------------------- */
/* FIXME This leaks somehow... Might be GTKs problem. */

bool
GtkCharPage::on_query_skillview_tooltip (int x, int y, bool key,
    Glib::RefPtr<Gtk::Tooltip> const& tooltip)
{
  key = false;

  Gtk::TreeModel::Path path;
  Gtk::TreeViewDropPosition pos;

  bool exists = this->skill_view.get_dest_row_at_pos(x, y, path, pos);

  if (!exists)
    return false;

  Gtk::TreeIter iter = this->skill_store->get_iter(path);
  int skill_id = (*iter)[this->skill_cols.id];
  ApiCharSheetSkill* cskill = (*iter)[this->skill_cols.skill];

  if (skill_id < 0)
    return false;

  try
  {
    ApiSkillTreePtr tree = ApiSkillTree::request();
    ApiSkill const* skill = cskill->details;
    tooltip->set_icon(ImageStore::skill);

    std::stringstream ss;
    ss << "Name: " << skill->name << "\n"
        << "Attributes: " << ApiSkillTree::get_attrib_name(skill->primary)
        << " / " << ApiSkillTree::get_attrib_name(skill->secondary) << "\n";

    if (cskill->level != 5)
    {
      int sp_to_go = cskill->points_dest - cskill->points;
      double sppm = this->sheet->get_sppm_for_skill(skill);
      time_t secs_next_level = (time_t)(60.0 * (double)sp_to_go / sppm);
      std::string next_level_str = EveTime::get_string_for_timediff
          (secs_next_level, false);
      ss << "SP per hour: " << (int)(sppm * 60.0) << "\n"
          << "Training time: " << next_level_str << "\n";
    }

    ss  << "\n" << skill->desc;
    tooltip->set_text(ss.str());

    return true;
  }
  catch (...)
  {
  }

  return false;
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::on_skill_activated (Gtk::TreeModel::Path const& path,
    Gtk::TreeViewColumn* col)
{
  col = 0;

  Gtk::TreeIter iter = this->skill_store->get_iter(path);
  int skill_id = (*iter)[this->skill_cols.id];

  if (skill_id >= 0)
  {
    /* It's a skill. */
    GuiSkill* skillgui = new GuiSkill();
    skillgui->set_skill(skill_id);
  }
  else
  {
    /* It's probably a skill group. Expand/collapse. */
    if (this->skill_view.row_expanded(path))
      this->skill_view.collapse_row(path);
    else
      this->skill_view.expand_row(path, true);
  }
}

/* ---------------------------------------------------------------- */

bool
GtkCharPage::on_live_sp_value_update (void)
{
  double level_sp, total_sp, fraction;
  bool success = this->calc_live_values(level_sp, total_sp, fraction);
  if (!success)
    return true;

  this->live_sp_label.set_text(Helpers::get_dotted_str_from_uint
      ((unsigned int)level_sp) + " SP ("
      + Helpers::get_string_from_double(fraction * 100.0, 1) + "%)");

  this->skill_points_label.set_text(Helpers::get_dotted_str_from_uint
      (this->skill_info.total_sp + (unsigned int)level_sp));

  (*this->skill_info.tree_skill_iter)[this->skill_cols.points]
      = Helpers::get_dotted_str_from_uint((unsigned int)total_sp);

  (*this->skill_info.tree_group_iter)[this->skill_cols.points]
      = Helpers::get_dotted_str_from_uint
      (this->skill_info.skill_group_sp + (unsigned int)level_sp);

  return true;
}

/* ---------------------------------------------------------------- */

bool
GtkCharPage::on_live_sp_image_update (void)
{
  double level_sp, total_sp, fraction;
  bool success = this->calc_live_values(level_sp, total_sp, fraction);
  if (!success)
    return true;

  Glib::RefPtr<Gdk::Pixbuf> new_icon = ImageStore::skill_progress
      (this->skill_info.char_skill->level, fraction);

  (*this->skill_info.tree_skill_iter)[this->skill_cols.level] = new_icon;

  return true;
}

/* ---------------------------------------------------------------- */

bool
GtkCharPage::calc_live_values (double& level_sp, double& total_sp, double& frac)
{
  if (!this->training->valid
      || !this->training->in_training
      || !this->sheet->valid
      || this->skill_info.char_skill == 0)
    return false;

  ApiCharSheetSkill* skill = this->skill_info.char_skill;

  time_t evetime = EveTime::get_eve_time();
  time_t finish = this->training->end_time_t;
  time_t diff = finish - evetime;

  double spps = this->skill_info.sp_per_hour / 3600.0;

  /* Assign values. */
  total_sp = skill->points_dest - diff * spps;
  level_sp = total_sp - skill->points_start;
  frac = level_sp / (skill->points_dest - skill->points_start);

  return true;
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::on_skilltree_error (std::string const& e)
{
  std::cout << "Error requesting skill tree: " << e << std::endl;

  this->info_display.append(INFO_ERROR,
      "Error requesting skill tree!",
      "There was an error while parsing the skill tree. "
      "Reasons might be: The file was not found, the file "
      "is currupted, the file uses a new syntax unknown "
      "to GtkEveMon. The error message is:\n\n" + e);

  #if 0
  Gtk::MessageDialog md("Error parsing skill tree!",
      false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
  md.set_secondary_text("There was an error while parsing the "
      "skill tree. Reasons might be: The file was not found, "
      "the file is currupted, the file was updated and uses "
      "a new syntax unknown to GtkEveMon. "
      "The error message is:\n\n" + Glib::locale_to_utf8(e));
  md.set_title("Error - GtkEveMon");
  md.set_transient_for(*this->parent_window);
  md.run();
  #endif
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::on_charsheet_error (std::string const& e)
{
  std::cout << "Error requesting char sheet: " << e << std::endl;

  this->info_display.append(INFO_ERROR,
      "Error requesting character sheet!",
      "There was an error while requesting the character "
      "sheet from the EVE API. The EVE API is either offline, or the "
      "requested document is not understood by GtkEveMon. "
      "The error message is:\n\n" + e);

  #if 0
  Gtk::MessageDialog md("Error retrieving character sheet!",
      false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
  md.set_secondary_text("There was an error while requesting the character "
      "sheet from the EVE API. The EVE API is either offline, or the "
      "requested document is not understood by GtkEveMon. "
      "The error message is:\n\n" + Glib::locale_to_utf8(e));
  md.set_title("Error - GtkEveMon");
  md.set_transient_for(*this->parent_window);
  md.run();
  #endif
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::on_intraining_error (std::string const& e)
{
  this->training_label.set_text("Error requesting!");

  std::cout << "Error requesting training sheet: " << e << std::endl;

  this->info_display.append(INFO_ERROR,
      "Error requesting training sheet!",
      "There was an error while requesting the training "
      "sheet from the EVE API. The EVE API is either offline, or the "
      "requested document is not understood by GtkEveMon. "
      "The error message is:\n\n" + e);

  #if 0
  Gtk::MessageDialog md("Error retrieving training sheet!",
      false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
  md.set_secondary_text("There was an error while requesting the training "
      "sheet from the EVE API. The EVE API is either offline, or the "
      "requested document is not understood by GtkEveMon. "
      "The error message is:\n\n" + Glib::locale_to_utf8(e));
  md.set_title("Error - GtkEveMon");
  md.set_transient_for(*this->parent_window);
  md.run();
  #endif
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::on_info_clicked (void)
{
  std::string char_cached("<unknown>");
  std::string train_cached("<unknown>");

  if (this->sheet->valid)
    char_cached = EveTime::get_gm_time_string
        (this->sheet->get_cached_until_t());

  if (this->training->valid)
    train_cached = EveTime::get_gm_time_string
        (this->training->get_cached_until_t());

  Gtk::MessageDialog md("Information about cached sheets",
      false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
  md.set_secondary_text(
      "Character sheet expires at " + char_cached + "\n"
      "Training sheet expires at " + train_cached);
  md.set_title("Cache Status - GtkEveMon");
  md.set_transient_for(*this->parent_window);
  md.run();
}

/* ---------------------------------------------------------------- */

std::string
GtkCharPage::get_char_name (void)
{
  if (this->sheet->valid)
    return this->sheet->name;
  else
    return this->character.char_id;
}

/* ---------------------------------------------------------------- */

std::string
GtkCharPage::get_tooltip_text (bool detailed)
{
  if (!this->sheet->valid)
    return "";

  std::string ret;

  ret += this->sheet->name;
  ret += " - ";

  if (this->training->valid && this->training->in_training)
  {
    ret += this->get_skill_remaining(true);
    if (detailed)
    {
      ret += " - ";
      ret += this->get_skill_in_training();
    }
  }
  else
    ret += "Not training!";

  return ret;
}

/* ---------------------------------------------------------------- */

std::string
GtkCharPage::get_skill_in_training (void)
{
  if (this->training->valid && this->training->in_training)
  {
    int skill_id = this->training->skill;
    int to_level = this->training->to_level;
    std::string to_level_str = Helpers::get_roman_from_int(to_level);
    std::string skill_str;

    try
    {
      ApiSkillTreePtr skills = ApiSkillTree::request();
      skill_str = skills->get_skill_for_id(skill_id)->name;
    }
    catch (Exception& e)
    {
      /* This happens if the ID is not found. */
      skill_str = Helpers::get_string_from_int(skill_id);
    }

    return skill_str + " " + to_level_str;
  }
  else
  {
    return "No skill in training!";
  }
}

/* ---------------------------------------------------------------- */

double
GtkCharPage::get_spph_in_training (void)
{
  if (!this->training->valid
      || !this->training->in_training
      || !this->sheet->valid)
    return 0;

  try
  {
    int skill_id = this->training->skill;
    ApiSkillTreePtr skills = ApiSkillTree::request();
    ApiSkill const* skill = skills->get_skill_for_id(skill_id);
    return this->sheet->get_sppm_for_skill(skill) * 60.0;
  }
  catch (Exception& e)
  {
    /* This happens if the ID is not found. */
    return 0;
  }
}

/* ---------------------------------------------------------------- */

std::string
GtkCharPage::get_skill_remaining (bool slim)
{
  if (!this->training->valid || !this->training->in_training)
    return "No training information!";

  time_t evetime = EveTime::get_eve_time();
  time_t finish = this->training->end_time_t;
  time_t diff = finish - evetime;

  return EveTime::get_string_for_timediff(diff, slim);
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::open_skill_planner (void)
{
  if (!this->sheet->valid)
  {
    this->info_display.append(INFO_WARNING, "Cannot open the skill "
        "planner without a valid character sheet!");
    return;
  }

  GuiSkillPlanner* planner = new GuiSkillPlanner();
  planner->set_character(this->sheet);
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::open_source_viewer (void)
{
  if (!this->sheet->valid || !this->training->valid)
  {
    this->info_display.append(INFO_WARNING, "Cannot open the source "
        "viewer without valid sheets!");
    return;
  }

  GuiXmlSource* window = new GuiXmlSource();
  window->append(this->sheet->get_http_data(), "CharacterSheet.xml");
  window->append(this->training->get_http_data(), "InTraining.xml");
}
