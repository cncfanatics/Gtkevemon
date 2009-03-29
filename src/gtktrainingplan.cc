#include <iostream>
#include <gtkmm/stock.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/table.h>
#include <gtkmm/scrolledwindow.h>

#include "helpers.h"
#include "evetime.h"
#include "xmltrainingplan.h"
#include "gtkcolumnsbase.h"
#include "gtkportrait.h"
#include "gtkhelpers.h"
#include "gtkconfwidgets.h"
#include "gtkdefines.h"
#include "imagestore.h"
#include "gtktrainingplan.h"

GtkSkillList::GtkSkillList (void)
{
  ApiSkillTreePtr tree = ApiSkillTree::request();

  //this->append_skill(tree->get_skill_for_id(3369), 3);
  //this->append_skill(tree->get_skill_for_id(3369), 4);
  //this->append_skill(tree->get_skill_for_id(3369), 5);
}

/* ---------------------------------------------------------------- */

void
GtkSkillList::set_character (ApiCharSheetPtr charsheet)
{
  this->charsheet = charsheet;
}

/* ---------------------------------------------------------------- */

void
GtkSkillList::set_training (ApiInTrainingPtr training)
{
  this->training = training;
}

/* ---------------------------------------------------------------- */

void
GtkSkillList::append_skill (ApiSkill const* skill, int level)
{
  this->append_skill(skill, level, true);
}

/* ---------------------------------------------------------------- */

void
GtkSkillList::append_skill (ApiSkill const* skill, int level, bool objective)
{
  /* Check if skill is already there. */
  if (this->has_plan_skill(skill, level, objective))
    return;

  ApiCharSheetSkill* cskill = this->charsheet->get_skill_for_id(skill->id);
  int char_level = cskill ? cskill->level : 0;

  /* Also skip skill if already char has it. */
  if (!objective && char_level >= level)
    return;

  GtkSkillInfo info;
  info.skill = skill;
  info.is_objective = objective;
  info.plan_level = level;

  if (level < 1)
  {
    return;
  }
  else if (level == 1)
  {
    ApiSkillTreePtr tree = ApiSkillTree::request();
    /* Append dependencies. */
    for (unsigned int i = 0; i < skill->deps.size(); ++i)
    {
      ApiSkill const* s = tree->get_skill_for_id(skill->deps[i].first);
      this->append_skill(s, skill->deps[i].second, false);
    }
  }
  else
  {
    /* Append previous levels. */
    this->append_skill(skill, level - 1, false);
  }

  this->push_back(info);
}

/* ---------------------------------------------------------------- */

void
GtkSkillList::move_skill (unsigned int from, unsigned int to)
{
  this->insert_skill(to, GtkSkillInfo());
  this->at(to) = this->at(from);
  this->delete_skill(from);
}

/* ---------------------------------------------------------------- */

void
GtkSkillList::insert_skill (unsigned int pos, GtkSkillInfo const& info)
{
  this->insert(this->begin() + pos, info);
}

/* ---------------------------------------------------------------- */

void
GtkSkillList::delete_skill (unsigned int index)
{
  this->erase(this->begin() + index);
}

/* ---------------------------------------------------------------- */

void
GtkSkillList::release_skill (unsigned int index)
{
  this->at(index).is_objective = false;

  /* Remove dependencies. */
  bool items_changed = false;
  do
  {
    items_changed = false;
    /* Go bottom up and remove unneded skills. */
    for (int i = (int)this->size() - 1; i >= 0; --i)
    {
      GtkSkillInfo& info = this->at(i);
      if (!info.is_objective && !this->is_dependency(i))
      {
        this->delete_skill(i);
        items_changed = true;
      }
    }
  }
  while (items_changed);
}

/* ---------------------------------------------------------------- */

void
GtkSkillList::calc_details (void)
{
  int train_skill = -1;
  int train_level = -1;
  if (this->training.get() != 0 && this->training->valid
      && this->training->in_training)
  {
    train_skill = this->training->skill;
    train_level = this->training->to_level;
  }

  /* Cached values for time calculations. */
  time_t now = EveTime::get_local_time();
  time_t now_eve = EveTime::get_eve_time();
  time_t duration = 0;

  /* Attribute values. */
  unsigned int training_level = this->charsheet->get_level_for_skill(3374);
  ApiCharAttribs attribs = this->charsheet->total;

  /* Go through list and do mighty things. Caching the cskill variable
   * will greatly reduce relookup of the charsheet skill. */
  ApiCharSheetSkill* cskill = 0;
  for (unsigned int i = 0; i < this->size(); ++i)
  {
    GtkSkillInfo& info = this->at(i);
    ApiSkill const* skill = info.skill;

    /* Only relookup the character skill if we really need to. */
    if (cskill == 0 || skill->id != cskill->id)
      cskill = this->charsheet->get_skill_for_id(skill->id);

    /* Update the skill icon. */
    if (skill->id == train_skill && info.plan_level == train_level)
      this->at(i).skill_icon = SKILL_STATUS_TRAINING;
    else if (this->has_char_skill(skill, info.plan_level))
      this->at(i).skill_icon = SKILL_STATUS_TRAINED;
    else if (this->has_char_dep_skills(skill, info.plan_level))
      this->at(i).skill_icon = SKILL_STATUS_TRAINABLE;
    else if (this->has_plan_dep_skills(i))
      this->at(i).skill_icon = SKILL_STATUS_UNTRAINABLE;
    else
      this->at(i).skill_icon = SKILL_STATUS_MISSING_DEPS;

    /* Cache if the current skill is in training. */
    bool active = (skill->id == train_skill && info.plan_level == train_level);

    /* SP per second. */
    unsigned int spph = this->charsheet->get_spph_for_skill(skill, attribs);
    double spps = spph / 3600.0;

    /* Start SP, dest SP and current SP. */
    int ssp = this->charsheet->calc_start_sp(info.plan_level - 1, skill->rank);
    int dsp = this->charsheet->calc_dest_sp(info.plan_level - 1, skill->rank);
    int csp = ssp;

    /* Set current SP only if in training or previous char level available. */
    if (active)
      csp = dsp - (int)((this->training->end_time_t - now_eve) * spps);
    else if (cskill != 0)
    {
      if (cskill->level + 1 == info.plan_level)
        csp = cskill->points;
      else if (cskill->level >= info.plan_level)
        csp = dsp;
    }

    time_t timediff = (time_t)((double)(dsp - csp) / spps);
    info.start_time = now + duration;
    info.finish_time = now + duration + timediff;
    info.train_duration = duration + timediff;
    info.skill_duration = timediff;
    info.completed = (double)(csp - ssp) / (double)(dsp - ssp);
    info.spph = spph;

    duration += timediff;

    /* If the current skill is a training skill not already known,
     * it has impact on the SP/h. Apply this impact. */
    if (cskill == 0 || cskill->level < info.plan_level)
    {
      this->apply_attributes(skill, attribs, training_level);
      if (skill->id == 3374) /* 3374 is the learning skill. */
        training_level += 1;
    }
  }
}

/* ---------------------------------------------------------------- */

void
GtkSkillList::cleanup_skills (void)
{
  for (int i = (int)this->size() - 1; i >= 0; --i)
    if (this->has_char_skill(this->at(i).skill, this->at(i).plan_level))
      this->delete_skill(i);
}

/* ---------------------------------------------------------------- */

bool
GtkSkillList::has_plan_dep_skills (unsigned int index)
{
  ApiSkill const* skill = this->at(index).skill;
  int plan_level = this->at(index).plan_level;

  /* Check for previous level for level > 1. */
  if (plan_level > 1)
  {
    for (int j = (int)index - 1; j >= 0; --j)
      if (this->at(j).skill == skill
          && this->at(j).plan_level == plan_level - 1)
        return true;
    return false;
  }

  /* Check for skill deps in the list. */
  for (int i = 0; i < (int)skill->deps.size(); ++i)
  {
    bool has_this_dep = false;
    for (int j = (int)index - 1; j >= 0 && !has_this_dep; --j)
    {
      if (this->at(j).skill->id == skill->deps[i].first
          && this->at(j).plan_level >= skill->deps[i].second)
        has_this_dep = true;

      if (this->charsheet->get_level_for_skill
          (skill->deps[i].first) >= skill->deps[i].second)
        has_this_dep = true;
    }

    if (!has_this_dep)
      return false;
  }

  return true;
}

/* ---------------------------------------------------------------- */

bool
GtkSkillList::has_char_dep_skills (ApiSkill const* skill, int level)
{
  int char_level = this->charsheet->get_level_for_skill(skill->id);

  /* Check if previous level of skill is available. */
  if (level > 1)
  {
    if (char_level >= level - 1)
      return true;
    else
      return false;
  }

  /* Check if deps are available. */
  for (unsigned int i = 0; i < skill->deps.size(); ++i)
  {
    int dep_level = this->charsheet->get_level_for_skill(skill->deps[i].first);
    if (dep_level < skill->deps[i].second)
      return false;
  }

  return true;
}

/* ---------------------------------------------------------------- */

bool
GtkSkillList::has_char_skill (ApiSkill const* skill, int level)
{
  //for (unsigned int i = 0; i < this->charsheet->skills.size(); ++i)
  if (this->charsheet->get_level_for_skill(skill->id) >= level)
    return true;

  return false;
}

/* ---------------------------------------------------------------- */

bool
GtkSkillList::has_plan_skill (ApiSkill const* skill, int level,
    bool make_objective)
{
  for (unsigned int i = 0; i < this->size(); ++i)
    if (this->at(i).skill == skill && this->at(i).plan_level == level)
    {
      if (make_objective)
        this->at(i).is_objective = true;

      return true;
    }

  return false;
}

/* ---------------------------------------------------------------- */

bool
GtkSkillList::is_dependency (unsigned int index)
{
  for (unsigned int i = 0; i < this->size(); ++i)
  {
    if (i == index)
      continue;

    if (this->at(i).skill == this->at(index).skill
        && this->at(i).plan_level - 1 == this->at(index).plan_level)
      return true;

    for (unsigned int j = 0; j < this->at(i).skill->deps.size(); ++j)
    {
      if (this->at(i).skill->deps[j].first == this->at(index).skill->id
          && this->at(i).skill->deps[j].second == this->at(index).plan_level)
        return true;
    }
  }

  return false;
}

/* ---------------------------------------------------------------- */

void
GtkSkillList::apply_attributes (ApiSkill const* skill, ApiCharAttribs& attribs,
    unsigned int training_level)
{
  double factor = 1 + training_level * 0.02;

  if (skill->id == 3377 || skill->id == 12376)
    attribs.intl += factor;
  else if (skill->id == 3379 || skill->id == 12387)
    attribs.per += factor;
  else if (skill->id == 3376 || skill->id == 12383)
    attribs.cha += factor;
  else if (skill->id == 3378 || skill->id == 12385)
    attribs.mem += factor;
  else if (skill->id == 3375 || skill->id == 12386)
    attribs.wil += factor;
  else if (skill->id == 3374)
  {
    double rescale = (1 + (training_level + 1) * 0.02) / factor;
    attribs.intl *= rescale;
    attribs.per *= rescale;
    attribs.cha *= rescale;
    attribs.mem *= rescale;
    attribs.wil *= rescale;
  }
}

/* ================================================================ */

GtkTreeModelColumns::GtkTreeModelColumns (void)
{
  this->add(this->skill_index);
  this->add(this->objective);
  this->add(this->skill_icon);
  this->add(this->skill_name);
  this->add(this->train_duration);
  this->add(this->skill_duration);
  this->add(this->completed);
  this->add(this->attributes);
  this->add(this->est_start);
  this->add(this->est_finish);
  this->add(this->spph);
}

/* ================================================================ */

GtkTreeViewColumns::GtkTreeViewColumns (Gtk::TreeView* view,
    GtkTreeModelColumns* cols)
  : GtkColumnsBase(view),
    objective("Objective", cols->objective),
    //skill_name("Name, level, rank", cols->skill_name),
    train_duration("Training", cols->train_duration),
    skill_duration("Duration", cols->skill_duration),
    completed("Done", cols->completed),
    attributes("Attribs", cols->attributes),
    est_start("Est. start", cols->est_start),
    est_finish("Est. finish", cols->est_finish),
    spph("SP/h", cols->spph)
{
  this->skill_name.set_title("Name, level, rank");
  this->skill_name.pack_start(cols->skill_icon, false);
  this->skill_name.pack_start(cols->skill_name, true);

  this->objective.set_resizable(false);
  Gtk::CellRendererToggle* objective_col = dynamic_cast
      <Gtk::CellRendererToggle*>(this->objective.get_first_cell_renderer());
  objective_col->set_property("activatable", true);

  this->append_column(&this->objective, GtkColumnOptions
      (false, true, false, ImageStore::columnconf[1]));
  this->append_column(&this->skill_name,
      GtkColumnOptions(false, true, true));
  this->append_column(&this->train_duration,
      GtkColumnOptions(true, true, true));
  this->append_column(&this->skill_duration,
      GtkColumnOptions(true, true, true));
  this->append_column(&this->completed,
      GtkColumnOptions(true, true, true));
  this->append_column(&this->attributes,
      GtkColumnOptions(true, true, true));
  this->append_column(&this->est_start,
      GtkColumnOptions(true, true, true));
  this->append_column(&this->est_finish,
      GtkColumnOptions(true, true, true));
  this->append_column(&this->spph,
      GtkColumnOptions(true, true, true));

  this->skill_name.set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
  this->skill_name.set_fixed_width(75);
  this->skill_name.set_expand(true);
  this->train_duration.set_resizable(false);
  this->skill_duration.set_resizable(false);
  this->completed.set_resizable(false);
  this->completed.get_first_cell_renderer()->set_property("xalign", 1.0f);
  this->attributes.set_resizable(false);
  this->est_start.set_resizable(false);
  this->est_finish.set_resizable(false);
  this->spph.set_resizable(false);
  this->spph.get_first_cell_renderer()->set_property("xalign", 1.0);
}

/* ================================================================ */

GtkTrainingPlan::GtkTrainingPlan (void)
  : Gtk::VBox(false, 5),
    liststore(Gtk::ListStore::create(cols)),
    treeview(liststore),
    viewcols(&treeview, &cols)
{
  /* Setup members. */
  this->delete_plan_but.set_image(*MK_IMG
      (Gtk::Stock::DELETE, Gtk::ICON_SIZE_MENU));
  this->create_plan_but.set_image(*MK_IMG
      (Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU));
  this->rename_plan_but.set_image(*MK_IMG
      (Gtk::Stock::EDIT, Gtk::ICON_SIZE_MENU));
  this->export_plan_but.set_image(*MK_IMG
      (Gtk::Stock::SAVE_AS, Gtk::ICON_SIZE_MENU));
  this->import_plan_but.set_image(*MK_IMG
      (Gtk::Stock::REVERT_TO_SAVED, Gtk::ICON_SIZE_MENU));
  //this->delete_plan_but.set_relief(Gtk::RELIEF_NONE);
  //this->create_plan_but.set_relief(Gtk::RELIEF_NONE);
  //this->rename_plan_but.set_relief(Gtk::RELIEF_NONE);
  this->export_plan_but.set_label("Export");
  this->import_plan_but.set_label("Import");

  this->clean_plan_but.set_image(*MK_IMG
      (Gtk::Stock::CLEAR, Gtk::ICON_SIZE_MENU));
  this->clean_plan_but.set_label("Clean finished");
  this->column_conf_but.set_image(*MK_IMG_PB(ImageStore::columnconf[0]));
  this->column_conf_but.set_label("Configure columns");

  this->total_time.set_text("n/a");
  this->total_time.set_alignment(Gtk::ALIGN_LEFT);

  this->reorder_new_index = -1;
  //this->clean_plan_but.set_label("Clean up");

  /* Setup treeview. */
  //this->treeview.get_selection()->set_mode(Gtk::SELECTION_MULIPLE);
  this->treeview.set_headers_visible(true);
  this->treeview.set_rules_hint(true);
  this->treeview.set_reorderable(true);
  //this->viewcols.set_format("+0 +1 +2 +3 -4 -5 -6 -7 -8");
  this->updating_liststore = false;

  Gtk::ScrolledWindow* scwin = MK_SCWIN;
  scwin->add(this->treeview);
  scwin->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
  scwin->set_shadow_type(Gtk::SHADOW_ETCHED_IN);

  /* Create GUI. */
  Gtk::HBox* button_box = MK_HBOX;
  button_box->pack_start(this->clean_plan_but, false, false, 0);
  button_box->pack_start(this->column_conf_but, false, false, 0);
  button_box->pack_end(this->import_plan_but, false, false, 0);
  button_box->pack_end(this->export_plan_but, false, false, 0);
  Gtk::VBox* button_vbox = MK_VBOX;
  button_vbox->pack_end(*button_box, false, false, 0);

  Gtk::HBox* section_selection_box = MK_HBOX0;
  section_selection_box->pack_start(this->plan_selection, true, true, 0);
  section_selection_box->pack_start(this->rename_plan_but, false, false, 0);
  section_selection_box->pack_start(this->create_plan_but, false, false, 0);
  section_selection_box->pack_start(this->delete_plan_but, false, false, 0);

  Gtk::Label* select_label = MK_LABEL("Training plan:");
  Gtk::Label* time_label = MK_LABEL("Total time:");
  select_label->set_alignment(Gtk::ALIGN_LEFT);
  time_label->set_alignment(Gtk::ALIGN_LEFT);

  Gtk::Table* gui_table = MK_TABLE(3, 3);
  gui_table->set_col_spacings(5);
  gui_table->set_row_spacings(5);
  gui_table->attach(this->portrait, 0, 1, 0, 3,
      Gtk::SHRINK, Gtk::SHRINK);
  gui_table->attach(*select_label, 1, 2, 0, 1,
      Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK);
  gui_table->attach(*section_selection_box, 2, 3, 0, 1,
      Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK);
  gui_table->attach(*time_label, 1, 2, 1, 2,
      Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK);
  gui_table->attach(this->total_time, 2, 3, 1, 2,
      Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK);
  gui_table->attach(*button_vbox, 1, 3, 2, 3,
      Gtk::EXPAND | Gtk::FILL, Gtk::EXPAND | Gtk::FILL);

  this->delete_plan_but.set_tooltip_text("Delete the current plan");
  this->create_plan_but.set_tooltip_text("Create a new plan");
  this->rename_plan_but.set_tooltip_text("Rename the current plan");
  this->clean_plan_but.set_tooltip_text("Remove finished skills");
  this->column_conf_but.set_tooltip_text("Configure training plan columns");
  this->export_plan_but.set_tooltip_text("Export training plan");
  this->import_plan_but.set_tooltip_text("Import training plan");

  this->pack_start(*gui_table, false, false, 0);
  this->pack_start(*scwin, true, true, 0);
  this->set_border_width(5);

  this->plan_selection.signal_conf_section_changed().connect
      (sigc::mem_fun(*this, &GtkTrainingPlan::skill_plan_changed));
  this->create_plan_but.signal_clicked().connect
      (sigc::mem_fun(*this, &GtkTrainingPlan::on_create_skill_plan));
  this->delete_plan_but.signal_clicked().connect
      (sigc::mem_fun(*this, &GtkTrainingPlan::on_delete_skill_plan));
  this->rename_plan_but.signal_clicked().connect
      (sigc::mem_fun(*this, &GtkTrainingPlan::on_rename_skill_plan));
  this->clean_plan_but.signal_clicked().connect
      (sigc::mem_fun(*this, &GtkTrainingPlan::on_cleanup_skill_plan));
  this->column_conf_but.signal_clicked().connect(sigc::mem_fun
      (this->viewcols, &GtkColumnsBase::toggle_edit_context));
  this->import_plan_but.signal_clicked().connect(sigc::mem_fun
      (*this, &GtkTrainingPlan::on_import_plan));
  this->export_plan_but.signal_clicked().connect(sigc::mem_fun
      (*this, &GtkTrainingPlan::on_export_plan));

  this->liststore->signal_row_inserted().connect
      (sigc::mem_fun(*this, &GtkTrainingPlan::on_row_inserted));
  this->liststore->signal_row_deleted().connect
      (sigc::mem_fun(*this, &GtkTrainingPlan::on_row_deleted));
  this->treeview.signal_row_activated().connect
      (sigc::mem_fun(*this, &GtkTrainingPlan::on_row_activated));
  this->viewcols.objective.signal_clicked().connect(sigc::mem_fun
      (this->viewcols, &GtkColumnsBase::toggle_edit_context));
  dynamic_cast<Gtk::CellRendererToggle*>(this->viewcols.objective
      .get_first_cell_renderer())->signal_toggled().connect(sigc::mem_fun
      (*this, &GtkTrainingPlan::on_objective_toggled));
  this->treeview.signal_query_tooltip().connect(sigc::mem_fun
      (*this, &GtkTrainingPlan::on_query_skillview_tooltip));
  this->treeview.set_has_tooltip(true);

  Glib::signal_timeout().connect(sigc::mem_fun(*this,
      &GtkTrainingPlan::on_update_skill_time), PLANNER_SKILL_TIME_UPDATE);

  this->init_from_config();
  this->viewcols.setup_columns_normal();
}

/* ---------------------------------------------------------------- */

GtkTrainingPlan::~GtkTrainingPlan (void)
{
  this->save_current_plan();
  this->store_to_config();
  Config::save_to_file();
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::init_from_config (void)
{
  ConfSectionPtr planner = Config::conf.get_section("planner");
  ConfValuePtr columns_format = planner->get_value("columns_format");

  this->viewcols.set_format(**columns_format);
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::store_to_config (void)
{
  ConfSectionPtr planner = Config::conf.get_section("planner");
  ConfValuePtr columns_format = planner->get_value("columns_format");
  ConfValuePtr current_plan = planner->get_value("current_plan");

  columns_format->set(this->viewcols.get_format());
  current_plan->set(this->plan_selection.get_active_name());
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::set_character (ApiCharSheetPtr character)
{
  this->charsheet = character;

  this->skills.set_character(character);
  this->portrait.set(character->char_id);

  std::string key_name = "plans." + character->char_id;
  ConfValuePtr current_plan = Config::conf.get_value("planner.current_plan");
  this->plan_selection.set_parent_config_section(key_name, **current_plan);
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::set_training (ApiInTrainingPtr training)
{
  this->training = training;
  this->skills.set_training(training);
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::append_skill (ApiSkill const* skill, int level)
{
  if (this->plan_section.get() == 0)
  {
    Gtk::MessageDialog dialog(*(Gtk::Window*)this->get_toplevel(),
        "Please create a training plan first", false,
        Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
    dialog.set_secondary_text("You have to create a training plan prior "
        "adding skills. Create your plan by clicking the \"new\" icon "
        "next to the plan selection.");
    dialog.set_title("Cannot add skill - GtkEveMon");
    //dialog.set_default_response(Gtk::RESPONSE_OK);
    dialog.run();
    return;
  }

  this->skills.append_skill(skill, level);
  this->update_plan(true);
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::on_objective_toggled (Glib::ustring const& path)
{
  Gtk::ListStore::iterator iter = this->liststore->get_iter(path);
  unsigned int index = (*iter)[this->cols.skill_index];
  if (this->skills[index].is_objective)
  {
    this->skills.release_skill(index);
    this->update_plan(true);
  }
  else
  {
    this->skills[index].is_objective = true;
    (*iter)[this->cols.objective] = true;
  }
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::update_plan (bool rebuild)
{
  if (this->charsheet.get() == 0 || !this->charsheet->valid)
    return;

  this->updating_liststore = true;
  this->skills.calc_details();

  Gtk::ListStore::iterator iter;
  if (rebuild)
  {
    /* For a complete rebuild the liststore is cleared and repopulated. */
    this->liststore->clear();
  }
  else
  {
    /* Without rebuilding only iteration is done. */
    iter = this->liststore->children().begin();
  }

  for (unsigned int i = 0; i < this->skills.size(); ++i)
  {
    GtkSkillInfo& info = this->skills[i];
    ApiSkill const* skill = info.skill;

    /* In rebuild mode rows are created instread of iterated. */
    if (rebuild)
    {
      iter = this->liststore->append();

      Glib::ustring skillname = skill->name;
      skillname += " " + Helpers::get_roman_from_int(info.plan_level);
      skillname += "  (" + Helpers::get_string_from_int(skill->rank) + ")";

      Glib::ustring attribs;
      attribs += ApiSkillTree::get_attrib_short_name(skill->primary);
      attribs += " / ";
      attribs += ApiSkillTree::get_attrib_short_name(skill->secondary);

      (*iter)[this->cols.skill_index] = i;
      (*iter)[this->cols.skill_name] = skillname;
      (*iter)[this->cols.objective] = info.is_objective;
      (*iter)[this->cols.attributes] = attribs;
    }

    /* The following cells are always updated. */
    Glib::ustring completed = Helpers::get_string_from_double
        (info.completed * 100.0, info.completed == 1.0 ? 0 : 1) + "%";

    (*iter)[this->cols.skill_icon] = ImageStore::skillplan[info.skill_icon];
    (*iter)[this->cols.completed] = completed;
    (*iter)[this->cols.spph] = info.spph;
    (*iter)[this->cols.train_duration]
        = EveTime::get_string_for_timediff(info.train_duration, true);
    (*iter)[this->cols.skill_duration]
        = EveTime::get_string_for_timediff(info.skill_duration, true);
    (*iter)[this->cols.est_start]
        = EveTime::get_local_time_string(info.start_time, true);
    (*iter)[this->cols.est_finish]
        = EveTime::get_local_time_string(info.finish_time, true);

    /* Advance the iterator if store is not rebuild. */
    if (!rebuild)
    {
      iter++;
    }
  }

  this->updating_liststore = false;

  /* Update the total time label in the GUI. */
  if (this->skills.empty())
    this->total_time.set_text("Skill plan is empty.");
  else
    this->total_time.set_text(EveTime::get_string_for_timediff
        (this->skills.back().train_duration, false));
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::skill_plan_changed (ConfSectionPtr section)
{
  if (section.get() == 0)
  {
    this->plan_section.reset();
    this->load_current_plan();
    this->rename_plan_but.set_sensitive(false);
    this->delete_plan_but.set_sensitive(false);
    this->treeview.set_sensitive(false);
  }
  else
  {
    this->save_current_plan();
    this->plan_section = section;
    this->load_current_plan();
    this->rename_plan_but.set_sensitive(true);
    this->delete_plan_but.set_sensitive(true);
    this->treeview.set_sensitive(true);
  }
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::load_current_plan (void)
{
  this->skills.clear();

  if (this->plan_section.get() == 0)
  {
    this->update_plan(true);
    return;
  }

  ApiSkillTreePtr tree = ApiSkillTree::request();
  for (conf_values_t::iterator iter = this->plan_section->values_begin();
      iter != this->plan_section->values_end(); iter++)
  {
    ConfValuePtr value = iter->second;
    std::string val = **value;
    size_t delim_pos = val.find_first_of(',');
    if (delim_pos == std::string::npos)
    {
      std::cout << "Error loading plan: Invalid skill plan entry" << std::endl;
      continue;
    }
    std::string id = val.substr(0, delim_pos);
    val = val.substr(delim_pos + 1);

    delim_pos = val.find_first_of(',');
    if (delim_pos == std::string::npos)
    {
      std::cout << "Error loading plan: Invalid skill plan entry" << std::endl;
      continue;
    }
    std::string level = val.substr(0, delim_pos);
    val = val.substr(delim_pos + 1);

    std::string objective = val;

    int skill_id = Helpers::get_int_from_string(id);
    int skill_level = Helpers::get_int_from_string(level);
    int is_objective = Helpers::get_int_from_string(objective);

    ApiSkill const* skill = tree->get_skill_for_id(skill_id);
    GtkSkillInfo info;
    info.skill = skill;
    info.plan_level = skill_level;
    info.is_objective = (bool)is_objective;
    this->skills.push_back(info);
  }

  this->update_plan(true);
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::save_current_plan (void)
{
  if (this->plan_section.get() == 0)
    return;

  this->plan_section->clear_values();
  for (unsigned int i = 0; i < this->skills.size(); ++i)
  {
    GtkSkillInfo& info = this->skills[i];
    std::string key = Helpers::get_string_from_uint(i + 100);
    std::string value = Helpers::get_string_from_int(info.skill->id);
    value += "," + Helpers::get_string_from_int(info.plan_level);
    value += "," + Helpers::get_string_from_int((int)info.is_objective);
    this->plan_section->add(key, ConfValue::create(value));
  }
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::on_create_skill_plan (void)
{
  Gtk::MessageDialog dialog(*(Gtk::Window*)this->get_toplevel(),
      "Enter name for the new training plan", false,
      Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK_CANCEL);
  dialog.set_secondary_text("Please enter the name for the new training "
      "plan. You can change the name later on or delete the plan if you "
      "don't need it anymore.");
  dialog.set_title("Create plan - GtkEveMon");
  dialog.set_default_response(Gtk::RESPONSE_OK);
  Gtk::VBox* dialog_box = dialog.get_vbox();

  Gtk::Entry new_name_entry;
  new_name_entry.set_activates_default(true);
  Gtk::HBox* entry_box = MK_HBOX;
  entry_box->pack_start(*MK_LABEL("Plan name:"), false, false, 0);
  entry_box->pack_start(new_name_entry, true, true, 0);
  dialog_box->pack_start(*entry_box, false, false, 0);
  dialog.show_all();
  int ret = dialog.run();

  switch (ret)
  {
    default:
    case Gtk::RESPONSE_NONE:
    case Gtk::RESPONSE_CANCEL:
      break;
    case Gtk::RESPONSE_OK:
      Glib::ustring text = new_name_entry.get_text();
      if (text.empty())
        break;
      this->plan_selection.create_new_section(text);
      break;
  }
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::on_rename_skill_plan (void)
{
  Glib::ustring old_text = this->plan_selection.get_active_name();
  if (old_text.empty())
    return;

  Gtk::MessageDialog dialog(*(Gtk::Window*)this->get_toplevel(),
      "Enter new name for the training plan", false,
      Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK_CANCEL);
  dialog.set_secondary_text("Remember to always fly safe!");
  dialog.set_title("Rename plan - GtkEveMon");
  dialog.set_default_response(Gtk::RESPONSE_OK);
  Gtk::VBox* dialog_box = dialog.get_vbox();

  Gtk::Entry new_name_entry;
  new_name_entry.set_activates_default(true);
  new_name_entry.set_text(old_text);
  new_name_entry.select_region(0, -1);
  Gtk::HBox* entry_box = MK_HBOX;
  entry_box->pack_start(*MK_LABEL("Plan name:"), false, false, 0);
  entry_box->pack_start(new_name_entry, true, true, 0);
  dialog_box->pack_start(*entry_box, false, false, 0);
  dialog.show_all();
  int ret = dialog.run();

  switch (ret)
  {
    default:
    case Gtk::RESPONSE_NONE:
    case Gtk::RESPONSE_CANCEL:
      break;
    case Gtk::RESPONSE_OK:
      Glib::ustring text = new_name_entry.get_text();
      try
      {
        this->plan_selection.rename_section(old_text, text);
      }
      catch (Exception& e)
      {
        std::cout << "Cannot rename plan: " << e << std::endl;
      }
      break;
  }
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::on_delete_skill_plan (void)
{
  std::string name = this->plan_selection.get_active_name();
  if (name.empty())
    return;

  Gtk::MessageDialog dialog(*(Gtk::Window*)this->get_toplevel(),
      "Do you really want to delete this plan?", false,
      Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
  dialog.set_secondary_text("You are about to delete a skill plan.\n"
      "Continue deleting <b>" + name + "</b>?", true);
  dialog.set_title("Delete plan - GtkEveMon");
  dialog.set_default_response(Gtk::RESPONSE_YES);

  int ret = dialog.run();

  switch (ret)
  {
    default:
    case Gtk::RESPONSE_NONE:
    case Gtk::RESPONSE_NO:
      break;
    case Gtk::RESPONSE_YES:
      this->plan_selection.delete_section(name);
      break;
  }
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::on_cleanup_skill_plan (void)
{
  this->skills.cleanup_skills();
  this->update_plan(true);
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::on_row_inserted (Gtk::TreePath const& path,
    Gtk::TreeModel::iterator const& iter)
{
  this->reorder_new_index = -1;

  if (this->updating_liststore || path.size() <= 0)
    return;

  if (iter) {} // Prevent warnings

  /* Remember the new position. Move the skill in the delete event. */
  this->reorder_new_index = path[0];
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::on_row_deleted (Gtk::TreePath const& path)
{
  if (this->updating_liststore || path.size() <= 0)
    return;

  if (this->reorder_new_index < 0)
    return;

  /* Pass old and new position and let the skill list do the work. */
  this->skills.move_skill(path[0], this->reorder_new_index);
  this->reorder_new_index = -1;

  this->update_plan(true);
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::on_row_activated (Gtk::TreeModel::Path const& path,
    Gtk::TreeViewColumn* column)
{
  column = 0; // Prevent warnings.
  Gtk::ListStore::iterator iter = this->liststore->get_iter(path);
  GtkSkillInfo& info = this->skills[(*iter)[this->cols.skill_index]];
  ApiSkill const* skill = info.skill;
  this->sig_skill_activated.emit(skill);
}

/* ---------------------------------------------------------------- */

bool
GtkTrainingPlan::on_update_skill_time (void)
{
  this->update_plan(false);
  return true;
}

/* ---------------------------------------------------------------- */

bool
GtkTrainingPlan::on_query_skillview_tooltip (int x, int y, bool key,
    Glib::RefPtr<Gtk::Tooltip> const& tooltip)
{
  key = false;

  Gtk::TreeModel::Path path;
  Gtk::TreeViewDropPosition pos;

  bool exists = this->treeview.get_dest_row_at_pos(x, y, path, pos);

  if (!exists)
    return false;

  Gtk::TreeIter iter = this->liststore->get_iter(path);
  unsigned int index = (*iter)[this->cols.skill_index];
  ApiSkill const* skill = this->skills[index].skill;

  if (skill == 0)
    return false;

  GtkHelpers::create_tooltip(tooltip, skill);
  return true;
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::on_import_plan (void)
{
  // TODO Ask if import to current plan or to new plan
  Gtk::Window* toplevel = (Gtk::Window*)this->get_toplevel();

  Gtk::MessageDialog dialog(*toplevel, "How to import the skill plan?",
      false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
  dialog.set_secondary_text("Please select if you want to import the "
      "skill plan into the current plan or into a brand new one.");
  dialog.set_title("Import plan - GtkEveMon");
  dialog.set_default_response(Gtk::RESPONSE_OK);

  /* Create widgets to select between new or existing plan. */
  Gtk::RadioButtonGroup rbg;
  Gtk::RadioButton* rb_current = MK_RADIO("Import into the current plan");
  Gtk::RadioButton* rb_new = MK_RADIO("Import into a new skill plan");
  rb_current->set_group(rbg);
  rb_new->set_group(rbg);
  Gtk::Label* label_new = MK_LABEL("Plan name:");
  Gtk::Entry* entry_new = MK_ENTRY;

  Gtk::Table* dialog_tbl = MK_TABLE(3, 2);
  dialog_tbl->attach(*rb_current, 1, 2, 0, 1, Gtk::EXPAND | Gtk::FILL);
  dialog_tbl->attach(*rb_new, 1, 2, 1, 2, Gtk::EXPAND | Gtk::FILL);
  dialog_tbl->attach(*label_new, 0, 1, 2, 3, Gtk::SHRINK | Gtk::FILL);
  dialog_tbl->attach(*entry_new, 1, 2, 2, 3, Gtk::EXPAND | Gtk::FILL);

  Gtk::VBox* dialog_box = dialog.get_vbox();
  dialog_box->pack_start(*dialog_tbl, false, false, 0);
  dialog_box->show_all();

  /* Check if there is a current plan, if not disable the choice. */
  if (this->plan_section.get() == 0)
  {
    rb_current->set_sensitive(false);
    rb_new->set_active(true);
  }

  int ret = dialog.run();
  dialog.hide();
  if (ret != Gtk::RESPONSE_OK)
    return;

  if (rb_new->get_active() && entry_new->get_text().empty())
  {
    Gtk::MessageDialog md(*toplevel, "No name for new plan given!",
        false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
    md.set_secondary_text("You selected to create a new plan but "
        "did not give a name for the new plan.");
    md.set_title("Import failed - GtkEveMon");
    md.run();
    return;
  }

  /* Open file chooser dialog. */
  Gtk::FileChooserDialog fcd(*toplevel, "Export skill plan...",
      Gtk::FILE_CHOOSER_ACTION_SAVE);
  fcd.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  fcd.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

  {
    Gtk::FileFilter filter;
    filter.add_pattern("*.emp");
    filter.set_name("EveMon Plan (*.emp)");
    fcd.add_filter(filter);
  }

  {
    Gtk::FileFilter filter;
    filter.add_pattern("*.xml");
    filter.set_name("EveMon Uncompressed Plan (*.xml)");
    fcd.add_filter(filter);
  }

  ret = fcd.run();
  if (ret != Gtk::RESPONSE_OK)
    return;

  fcd.hide();

  std::string filename = fcd.get_filename();
  XmlTrainingPlanImport plan_import;
  try
  {
    plan_import.import_from_file(filename);
  }
  catch (Exception& e)
  {
    Gtk::MessageDialog md(*toplevel, "Error reading the plan from file!",
        false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
    md.set_secondary_text("The import of the training plan failed. "
        "The error message was:\n" + e);
    md.set_title("Import failed - GtkEveMon");
    md.run();
    return;
  }

  /* Append loaded skills to a plan. */
  if (rb_new->get_active())
  {
    Glib::ustring plan_name = entry_new->get_text();
    this->plan_selection.create_new_section(plan_name);
  }

  TrainingPlan const& plan = plan_import.get_training_plan();
  std::cout << "Plan size: " << plan.size() << std::endl;
  for (std::size_t i = 0; i < plan.size(); ++i)
  {
    if (plan[i].prerequisite)
      continue;

    this->append_skill(plan[i].skill, plan[i].level);
  }
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::on_export_plan (void)
{
  Gtk::Window* toplevel = (Gtk::Window*)this->get_toplevel();

  Gtk::FileChooserDialog fcb(*toplevel, "Export skill plan...",
      Gtk::FILE_CHOOSER_ACTION_SAVE);
  fcb.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  fcb.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
  fcb.set_do_overwrite_confirmation(true);

  {
    Gtk::FileFilter filter;
    filter.add_pattern("*.emp");
    filter.set_name("EveMon Plan (*.emp)");
    fcb.add_filter(filter);
  }

  {
    Gtk::FileFilter filter;
    filter.add_pattern("*.xml");
    filter.set_name("EveMon Uncompressed Plan (*.xml)");
    fcb.add_filter(filter);
  }

  std::string filename;
  while (true)
  {
    int ret = fcb.run();
    if (ret != Gtk::RESPONSE_OK)
      return;

    filename = fcb.get_filename();
    bool filename_changed = false;

    if (filename.size() < 4)
    {
      filename += ".emp";
      filename_changed = true;
    }
    else
    {
      std::string extension = filename.substr(filename.size() - 4);
      if (extension != ".emp" && extension != ".xml")
      {
        filename += ".emp";
        filename_changed = true;
      }
    }

    if (filename_changed)
    {
      fcb.set_current_name(filename);
      fcb.response(Gtk::RESPONSE_OK);
    }
    else
      break;
  }

  fcb.hide();

  XmlTrainingPlanExport plan_export;
  for (std::size_t i = 0; i < this->skills.size(); ++i)
  {
    if (!this->skills[i].is_objective)
      continue;

    plan_export.append_training_item(XmlTrainingItem
        (this->skills[i].skill, this->skills[i].plan_level, false));
  }

  try
  {
    plan_export.write_to_file(filename);
  }
  catch (Exception& e)
  {
    Gtk::MessageDialog md(*toplevel, "Training plan export failed!",
        false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
    md.set_secondary_text("The export of the training plan failed. "
        "The error message was:\n" + e);
    md.set_title("Export failed - GtkEveMon");
    md.run();
  }
}
