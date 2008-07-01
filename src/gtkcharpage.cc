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
#include "gtkcharpage.h"

GtkCharPage::GtkCharPage (void)
  : Gtk::VBox(false, 5), patience_box(false, 5)
{
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

  this->patience_box.set_border_width(5);
  Gtk::Image* patience_image = Gtk::manage(new Gtk::Image
      (Gtk::Stock::DIALOG_INFO, Gtk::ICON_SIZE_DIALOG));
  Gtk::Label* patience_label = MK_LABEL
      ("The data is received from the API server.\n"
      "Please be patient. This may take a while.");
  patience_image->property_xalign() = 1.0f;
  patience_label->property_xalign() = 0.0f;
  this->patience_box.pack_start(*patience_image, true, true, 0);
  this->patience_box.pack_start(*patience_label, true, true, 0);
  this->patience_box.show_all();

  this->scwin.set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  this->scwin.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
  this->scwin.add(this->skill_view);
  this->scwin.show_all();

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

  Gtk::Button* close_but = Gtk::manage(new Gtk::Button);
  close_but->set_relief(Gtk::RELIEF_NONE);
  close_but->set_image(*Gtk::manage(new Gtk::Image
      (Gtk::Stock::CLOSE, Gtk::ICON_SIZE_BUTTON)));
  Gtk::HBox* close_but_box = MK_HBOX;
  close_but_box->pack_end(*close_but, false, false, 0);

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
  info_table->attach(*close_but_box, 6, 7, 0, 2,
      Gtk::FILL | Gtk::EXPAND, Gtk::SHRINK);

  this->set_patience_info();

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

  close_but->signal_clicked().connect(sigc::mem_fun
      (*this, &GtkCharPage::on_close_clicked));
  this->refresh_but.signal_clicked().connect(sigc::mem_fun
      (*this, &GtkCharPage::refresh_data));
  this->info_but.signal_clicked().connect(sigc::mem_fun
      (*this, &GtkCharPage::info_clicked));
  this->skill_view.signal_row_activated().connect(sigc::mem_fun
      (*this, &GtkCharPage::on_skill_activated));
  this->skill_view.set_has_tooltip(true);
  this->skill_view.signal_query_tooltip().connect(sigc::mem_fun
      (*this, &GtkCharPage::on_query_skillview_tooltip));

  Glib::signal_timeout().connect(sigc::mem_fun(*this,
      &GtkCharPage::update_remaining), CHARPAGE_REMAINING_UPDATE);
  Glib::signal_timeout().connect(sigc::mem_fun(*this,
      &GtkCharPage::on_live_sp_value_update), CHARPAGE_LIVE_SP_LABEL_UPDATE);
  Glib::signal_timeout().connect(sigc::mem_fun(*this,
      &GtkCharPage::on_live_sp_image_update), CHARPAGE_LIVE_SP_IMAGE_UPDATE);

  this->set_border_width(5);
  this->pack_start(*info_table, false, false, 0);
  this->pack_start(this->skill_frame, true, true, 0);
  this->pack_start(*train_table, false, false, 0);

  this->api_info_changed();

  this->show_all();
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::set_character (EveApiAuth const& character)
{
  this->character = character;
  this->char_image.set(this->character.char_id);

  this->set_patience_info();

  try
  { this->sheet = ApiCharSheet::create(this->character); }
  catch (Exception& e)
  { this->popup_charsheet_error(e); }

  try
  { this->training = ApiInTraining::create(this->character); }
  catch (Exception& e)
  { this->popup_intraining_error(e); }

  this->api_info_changed();
  this->set_skill_list();
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::update_gui (void)
{
  /* Set character information. */
  if (this->sheet.get() == 0)
  {
    this->char_name_label.set_text("<b>---</b>");
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

    this->attr_cha_label.set_text(Helpers::get_string_from_float
        (this->sheet->total_cha, 2));
    this->attr_int_label.set_text(Helpers::get_string_from_float
        (this->sheet->total_int, 2));
    this->attr_per_label.set_text(Helpers::get_string_from_float
        (this->sheet->total_per, 2));
    this->attr_mem_label.set_text(Helpers::get_string_from_float
        (this->sheet->total_mem, 2));
    this->attr_wil_label.set_text(Helpers::get_string_from_float
        (this->sheet->total_wil, 2));
  }

  this->update_skill_list();

  /* Set training information. */
  if (this->training.get() == 0)
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
      this->finish_eve_label.set_text(this->training->end_time);
      this->finish_local_label.set_text(EveTime::get_local_time_string
          (EveTime::adjust_local_time(this->training->end_time_t)));

      if (this->sheet.get() != 0)
        this->spph_label.set_text(Helpers::get_string_from_uint
            ((unsigned int)this->get_spph_in_training()) + " SP per hour");
      else
        this->spph_label.set_text("---");

      this->on_live_sp_value_update();
      this->on_live_sp_image_update();
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
GtkCharPage::refresh_data (void)
{
  if (this->sheet.get() == 0 || this->training.get() == 0)
  {
    this->set_character(this->character);
    return;
  }

  std::string evetime = EveTime::get_eve_time_string();
  std::string char_cached = this->sheet->get_cached_until();
  std::string train_cached = this->training->get_cached_until();

  bool update_char = true;
  bool update_training = true;

  if (evetime < char_cached)
    update_char = false;

  if (evetime < train_cached)
    update_training = false;

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
      case Gtk::RESPONSE_DELETE_EVENT:
      case Gtk::RESPONSE_NO:
        return;
      case Gtk::RESPONSE_YES:
        update_char = true;
        update_training = true;
        break;
      default:
        return;
    };
  }

  this->set_patience_info();

  try
  {
    if (update_char)
      this->sheet->refresh();
  }
  catch (Exception& e)
  { this->popup_charsheet_error(e); }

  try
  {
    if (update_training)
      this->training->refresh();
  }
  catch (Exception& e)
  { this->popup_intraining_error(e); }

  this->api_info_changed();
  this->set_skill_list();
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::api_info_changed (void)
{
  /* Update the cache for the skill in training. */
  if (this->training.get() != 0 && this->training->in_training
      && this->sheet.get() != 0)
  {
    this->skill_info.char_skill = this->sheet->get_skill_for_id
        (this->training->skill);
    this->skill_info.sp_per_hour = this->get_spph_in_training();
  }

  if (this->sheet.get() != 0)
  {
    this->skill_info.total_sp = 0;
    this->skill_info.skills_at_five = 0;

    for (unsigned int i = 0; i < this->sheet->skills.size(); ++i)
    {
      ApiCharSheetSkill& skill = this->sheet->skills[i];
      this->skill_info.total_sp += skill.points;
      this->skill_info.skills_at_five += (skill.level == 5 ? 1 : 0);
    }

    //std::cout << "Total SP are: " << this->skill_info.total_sp
    //    << " Skill: " << this->skill_info.char_skill->points
    //    << " Dest: " << this->skill_info.char_skill->points_dest << std::endl;
  }

  /* Refresh the GUI with the new information. */
  this->update_gui();
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::update_skill_list (void)
{
  this->skill_store->clear();

  if (this->sheet.get() == 0)
    return;

  std::vector<ApiCharSheetSkill>& skills = this->sheet->skills;
  ApiSkillTreePtr tree;

  try
  {
    tree = ApiSkillTree::request();
  }
  catch (Exception& e)
  {
    this->popup_skilltree_error(e);
    return;
  }

  /* Cache skill in training. */
  ApiSkill skill_training;
  skill_training.id = -1;
  skill_training.group = -1;
  if (this->training.get() != 0 && this->training->in_training)
    skill_training = tree->get_skill_from_id(this->training->skill);

  /* Append all groups to the store. Save their iterators for the children.
   * Format is <group_id, <model iter>, <group sp> >. */
  typedef std::map<int, std::pair<Gtk::TreeModel::iterator, int> > IterMapType;
  IterMapType iter_map;
  for (ApiSkillGroupMap::iterator iter = tree->groups.begin();
      iter != tree->groups.end(); iter++)
  {
    std::string name = iter->second.name;
    if (skill_training.group == iter->first)
      name += "  <i>(1 in training)</i>";

    Gtk::TreeModel::iterator siter = this->skill_store->append();
    (*siter)[this->skill_cols.name] = name;
    (*siter)[this->skill_cols.id] = -1;
    (*siter)[this->skill_cols.icon] = ImageStore::skillicons[0];
    iter_map.insert(std::make_pair(iter->first, std::make_pair(siter, 0)));
  }

  /* Append all skills to the skill groups. */
  for (unsigned int i = 0; i < skills.size(); ++i)
  {
    /* Get skill object. */
    ApiSkill const& skill = tree->get_skill_from_id(skills[i].id);

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
    (*iter->second.first)[this->skill_cols.points]
        = Helpers::get_dotted_str_from_int(iter->second.second);

    /* Update of the SkillInTrainingInfo. */
    if (iter->first == skill_training.group)
    {
      this->skill_info.skill_group_sp = iter->second.second;
      this->skill_info.tree_group_iter = iter->second.first;
    }
  }
}

/* ---------------------------------------------------------------- */

bool
GtkCharPage::update_remaining (void)
{
  if (this->training.get() != 0 && this->training->in_training)
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
  this->training->in_training = false;

  this->remaining_label.set_text("Completed!");
  this->finish_eve_label.set_text("---");
  this->finish_local_label.set_text("---");
  this->spph_label.set_text("0 SP per hour");
  this->live_sp_label.set_text("---");

  ConfValuePtr show_popup = Config::conf.get_value
      ("notifications.show_popup_dialog");
  ConfValuePtr show_tray = Config::conf.get_value
      ("notifications.show_tray_icon");

  if (show_tray->get_bool())
    this->create_tray_notify();

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

  if (skill_id < 0)
    return false;

  try
  {
    ApiSkillTreePtr tree = ApiSkillTree::request();
    ApiSkill const& skill = tree->get_skill_from_id(skill_id);
    tooltip->set_icon(ImageStore::skill);

    std::stringstream ss;
    ss << "Name: " << skill.name << "\n"
        << "Attributes: " << tree->get_attrib_name(skill.primary)
        << " / " << tree->get_attrib_name(skill.secondary) << "\n\n"
        << skill.desc;
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
  if (this->training.get() == 0
      || !this->training->in_training
      || this->sheet.get() == 0)
    return true;

  ApiCharSheetSkill* skill = this->skill_info.char_skill;

  time_t evetime = EveTime::get_eve_time();
  time_t finish = this->training->end_time_t;
  time_t diff = finish - evetime;

  double spps = this->skill_info.sp_per_hour / 3600.0;
  double sp = skill->points_dest - diff * spps - skill->points_start;
  double fraction = sp / (skill->points_dest - skill->points_start);

  unsigned int sp_total = skill->points_dest - (unsigned int)(diff * spps);

  this->live_sp_label.set_text(Helpers::get_dotted_str_from_uint
      ((unsigned int)sp) + /*" / " + Helpers::get_dotted_str_from_uint
      (skill->points_dest - skill->points_start) +*/ " SP ("
      + Helpers::get_string_from_double(fraction * 100.0, 1) + "%)");

  this->skill_points_label.set_text(Helpers::get_dotted_str_from_uint
      (this->skill_info.total_sp - skill->points + sp_total));

  (*this->skill_info.tree_skill_iter)[this->skill_cols.points]
      = Helpers::get_dotted_str_from_uint(sp_total);

  (*this->skill_info.tree_group_iter)[this->skill_cols.points]
      = Helpers::get_dotted_str_from_uint
      (this->skill_info.skill_group_sp - skill->points + sp_total);

  return true;
}

/* ---------------------------------------------------------------- */

bool
GtkCharPage::on_live_sp_image_update (void)
{
  if (this->training.get() == 0
      || !this->training->in_training
      || this->sheet.get() == 0)
    return true;

  ApiCharSheetSkill* skill = this->skill_info.char_skill;

  time_t evetime = EveTime::get_eve_time();
  time_t finish = this->training->end_time_t;
  time_t diff = finish - evetime;

  double spps = this->skill_info.sp_per_hour / 3600.0;
  double sp = skill->points_dest - diff * spps - skill->points_start;
  double fraction = sp / (skill->points_dest - skill->points_start);

  Glib::RefPtr<Gdk::Pixbuf> new_icon = ImageStore::skill_progress
      (this->skill_info.char_skill->level, fraction);

  (*this->skill_info.tree_skill_iter)[this->skill_cols.level] = new_icon;

  return true;
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::set_patience_info (void)
{
  this->skill_frame.remove();
  this->skill_frame.add(this->patience_box);
  this->skill_frame.set_shadow_type(Gtk::SHADOW_ETCHED_IN);

  while (Gtk::Main::events_pending())
    Gtk::Main::iteration();
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::set_skill_list (void)
{
  this->skill_frame.remove();
  this->skill_frame.add(this->scwin);
  this->skill_frame.set_shadow_type(Gtk::SHADOW_NONE);
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::popup_skilltree_error (Exception const& e)
{
  Gtk::MessageDialog md("Error parsing skill tree!",
      false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
  md.set_secondary_text("There was an error while parsing the "
      "skill tree. Reasons might be: The file was not found, "
      "the file is currupted, the file was updated and uses "
      "a new syntax unknown to GtkEveMon. "
      "The error message is:\n\n" + e);
  md.set_title("Error - GtkEveMon");
  md.set_transient_for(*this->parent_window);
  md.run();
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::popup_charsheet_error (Exception const& e)
{
  Gtk::MessageDialog md("Error retrieving character sheet!",
      false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
  md.set_secondary_text("There was an error while requesting the character "
      "sheet from the EVE API. The EVE API is either offline, or the "
      "requested document is not understood by GtkEveMon. "
      "The error message is:\n\n" + e);
  md.set_title("Error - GtkEveMon");
  md.set_transient_for(*this->parent_window);
  md.run();
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::popup_intraining_error (Exception const& e)
{
  Gtk::MessageDialog md("Error retrieving training sheet!",
      false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
  md.set_secondary_text("There was an error while requesting the training "
      "sheet from the EVE API. The EVE API is either offline, or the "
      "requested document is not understood by GtkEveMon. "
      "The error message is:\n\n" + e);
  md.set_title("Error - GtkEveMon");
  md.set_transient_for(*this->parent_window);
  md.run();
}

/* ---------------------------------------------------------------- */

void
GtkCharPage::info_clicked (void)
{
  std::string char_cached("<unknown>");
  std::string train_cached("<unknown>");

  if (this->sheet.get() != 0)
    char_cached = this->sheet->get_cached_until();

  if (this->training.get() != 0)
    train_cached = this->training->get_cached_until();

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
  if (this->sheet.get() != 0)
    return this->sheet->name;
  else
    return this->character.char_id;
}

/* ---------------------------------------------------------------- */

std::string
GtkCharPage::get_tooltip_text (void)
{
  std::string ret;
  if (this->sheet.get() != 0)
  {
    ret += this->sheet->name;
    ret += " - ";
    if (this->training.get() != 0 && this->training->in_training)
      ret += this->get_skill_remaining(true);
    else
      ret += "Not training!";
  }

  return ret;
}

/* ---------------------------------------------------------------- */

std::string
GtkCharPage::get_skill_in_training (void)
{
  if (this->training.get() != 0 && this->training->in_training)
  {
    int skill_id = this->training->skill;
    int to_level = this->training->to_level;
    std::string to_level_str = Helpers::get_roman_from_int(to_level);
    std::string skill_str;

    try
    {
      ApiSkillTreePtr skills = ApiSkillTree::request();
      skill_str = skills->get_skill_from_id(skill_id).name;
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
  if (this->training.get() == 0 || !this->training->in_training)
    return 0;

  if (this->sheet.get() == 0)
    return 0;

  /* Receive primary and secondary attribs. */
  ApiAttrib primary;
  ApiAttrib secondary;
  try
  {
    int skill_id = this->training->skill;
    ApiSkillTreePtr skills = ApiSkillTree::request();
    ApiSkill const& skill = skills->get_skill_from_id(skill_id);
    primary = skill.primary;
    secondary = skill.secondary;
  }
  catch (Exception& e)
  {
    /* This happens if the ID is not found. */
    return 0;
    //skill_str = Helpers::get_string_from_int(skill_id);
  }

  /* Get attribute values. */
  double pri;
  double sec;

  switch (primary)
  {
    case API_ATTRIB_INTELLIGENCE: pri = this->sheet->total_int; break;
    case API_ATTRIB_MEMORY:       pri = this->sheet->total_mem; break;
    case API_ATTRIB_CHARISMA:     pri = this->sheet->total_cha; break;
    case API_ATTRIB_PERCEPTION:   pri = this->sheet->total_per; break;
    case API_ATTRIB_WILLPOWER:    pri = this->sheet->total_wil; break;
    default: pri = 0.0;
  }

  switch (secondary)
  {
    case API_ATTRIB_INTELLIGENCE: sec = this->sheet->total_int; break;
    case API_ATTRIB_MEMORY:       sec = this->sheet->total_mem; break;
    case API_ATTRIB_CHARISMA:     sec = this->sheet->total_cha; break;
    case API_ATTRIB_PERCEPTION:   sec = this->sheet->total_per; break;
    case API_ATTRIB_WILLPOWER:    sec = this->sheet->total_wil; break;
    default: sec = 0.0;
  }

  /* Calculate SP/h */
  double spph = 60.0 * (pri + sec / 2.0);
  return spph;
}

/* ---------------------------------------------------------------- */

std::string
GtkCharPage::get_skill_remaining (bool slim)
{
  if (this->training.get() == 0 || !this->training->in_training)
    return "No training information!";

  int slim_count = 2;

  time_t evetime = EveTime::get_eve_time();
  time_t finish = this->training->end_time_t;
  time_t diff = finish - evetime;

  time_t seconds = diff % 60;
  diff /= 60;
  time_t minutes = diff % 60;
  diff /= 60;
  time_t hours = diff % 24;
  diff /= 24;
  time_t days = diff;

  std::stringstream ss;
  if (days > 0)
  {
    ss << (int)days << "d ";
    slim_count -= 1;
  }
  if (days > 0 || hours > 0)
  {
    ss << (int)hours << "h ";
    slim_count -= 1;
  }
  if ((!slim || slim_count > 0) && (days > 0 && hours > 0 || minutes > 0))
  {
    ss << (int)minutes << "m ";
    slim_count -= 1;
  }

  if (!slim || slim_count > 0)
    ss << (int)seconds << "s";

  return ss.str();
}

/* ---------------------------------------------------------------- */

EveApiAuth const&
GtkCharPage::get_character (void)
{
  return this->character;
}

/* ---------------------------------------------------------------- */

sigc::signal<void, EveApiAuth>&
GtkCharPage::signal_close_request (void)
{
  return this->sig_close_request;
}
