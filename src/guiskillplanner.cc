#include <iostream>
#include <gtkmm/box.h>
#include <gtkmm/table.h>
#include <gtkmm/paned.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/stock.h>
#include <gtkmm/image.h>
#include <gtkmm/notebook.h>
#include <gtkmm/separator.h>
#include <gtkmm/scrolledwindow.h>

#include "helpers.h"
#include "evetime.h"
#include "imagestore.h"
#include "gtkdefines.h"
#include "guiskillplanner.h"

GtkSkillHistory::GtkSkillHistory (void)
  : Gtk::HBox(false, 0)
{
  this->history_pos = 0;

  this->back_but.set_image(*MK_IMG(Gtk::Stock::GO_BACK, Gtk::ICON_SIZE_MENU));
  this->next_but.set_image(*MK_IMG(Gtk::Stock::GO_FORWARD,
      Gtk::ICON_SIZE_MENU));
  this->back_but.set_relief(Gtk::RELIEF_NONE);
  this->next_but.set_relief(Gtk::RELIEF_NONE);
  this->position_label.set_text("0/0");

  this->pack_start(this->back_but, false, false, 0);
  this->pack_start(this->position_label, false, false, 2);
  this->pack_start(this->next_but, false, false, 0);

  this->back_but.signal_clicked().connect(sigc::mem_fun
      (*this, &GtkSkillHistory::back_clicked));
  this->next_but.signal_clicked().connect(sigc::mem_fun
      (*this, &GtkSkillHistory::next_clicked));

  this->update_sensitive();
}

/* ---------------------------------------------------------------- */

void
GtkSkillHistory::append_skill (ApiSkill const* skill)
{
  this->history.push_back(skill);

  while (this->history.size() > HISTORY_MAX_SIZE)
    this->history.erase(this->history.begin());

  this->history_pos = this->history.size() - 1;

  this->update_pos_label();
  this->update_sensitive();
  this->sig_skill_changed.emit(this->history[this->history_pos]);
}

/* ---------------------------------------------------------------- */

void
GtkSkillHistory::update_sensitive (void)
{
  this->back_but.set_sensitive(this->history_pos > 0);
  this->next_but.set_sensitive(!this->history.empty()
      && this->history_pos < this->history.size() - 1);
}

/* ---------------------------------------------------------------- */

void
GtkSkillHistory::update_pos_label (void)
{
  this->position_label.set_text(Helpers::get_string_from_uint
      (this->history_pos + 1) + "/" + Helpers::get_string_from_uint
      (this->history.size()));
}

/* ---------------------------------------------------------------- */

void
GtkSkillHistory::back_clicked (void)
{
  if (this->history_pos > 0)
    this->history_pos -= 1;

  this->update_pos_label();
  this->update_sensitive();
  this->sig_skill_changed.emit(this->history[this->history_pos]);
}

/* ---------------------------------------------------------------- */

void
GtkSkillHistory::next_clicked (void)
{
  if (this->history_pos < this->history.size() -1)
    this->history_pos += 1;

  this->update_pos_label();
  this->update_sensitive();
  this->sig_skill_changed.emit(this->history[this->history_pos]);
}

/* ================================================================ */

GtkSkillDetails::GtkSkillDetails (void)
  : Gtk::VBox(false, 5),
    deps_store(Gtk::TreeStore::create(deps_cols)),
    deps_view(deps_store),
    skill_desc_buffer(Gtk::TextBuffer::create())
{
  Gtk::TreeViewColumn* deps_col_name = Gtk::manage(new Gtk::TreeViewColumn);
  deps_col_name->set_title("Name");
  deps_col_name->pack_start(this->deps_cols.icon, false);
  deps_col_name->pack_start(this->deps_cols.name, true);
  this->deps_view.append_column(*deps_col_name);
  this->deps_view.set_headers_visible(false);
  this->deps_view.get_selection()->set_mode(Gtk::SELECTION_SINGLE);
  //this->deps_view.set_show_expanders(false);
  this->deps_view.set_enable_tree_lines(true);

  this->skill_group.set_text("No skill selected");
  this->skill_group.property_xalign() = 0.0f;
  this->skill_name.property_xalign() = 0.0f;
  this->skill_primary.property_xalign() = 0.0f;
  this->skill_secondary.property_xalign() = 0.0f;
  for (unsigned int i = 0; i < 5; ++i)
    this->skill_level[i].property_xalign() = 0.0f;
  this->skill_level[4].property_yalign() = 0.0f;

  Gtk::VBox* details_skill_vbox = MK_VBOX0;
  details_skill_vbox->pack_start(this->skill_group, false, false, 0);
  details_skill_vbox->pack_start(this->skill_name, false, false, 0);

  Gtk::Button* queue_skill_but = MK_BUT0;
  queue_skill_but->set_relief(Gtk::RELIEF_NONE);
  queue_skill_but->set_image(*MK_IMG(Gtk::Stock::EDIT, Gtk::ICON_SIZE_MENU));

  Gtk::HBox* details_skill_box = MK_HBOX;
  details_skill_box->pack_start(*details_skill_vbox, true, true, 0);
  details_skill_box->pack_start(this->history, false, false, 0);
  //details_skill_box->pack_start(*queue_skill_but, false, false, 0);

  Gtk::TextView* text_view = Gtk::manage
      (new Gtk::TextView(this->skill_desc_buffer));
  text_view->set_editable(false);
  text_view->set_left_margin(3);
  text_view->set_wrap_mode(Gtk::WRAP_WORD);
  Gtk::ScrolledWindow* text_scwin = MK_SCWIN;
  text_scwin->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  text_scwin->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
  text_scwin->add(*text_view);

  Gtk::Table* details_table = Gtk::manage(new Gtk::Table(7, 2, false));
  details_table->set_col_spacings(5);
  details_table->attach(this->skill_primary, 0, 1, 0, 1,
      Gtk::FILL | Gtk::SHRINK, Gtk::SHRINK);
  details_table->attach(this->skill_secondary, 0, 1, 1, 2,
      Gtk::FILL | Gtk::SHRINK, Gtk::SHRINK);
  details_table->attach(this->skill_level[0], 0, 1, 2, 3,
      Gtk::FILL | Gtk::SHRINK, Gtk::SHRINK);
  details_table->attach(this->skill_level[1], 0, 1, 3, 4,
      Gtk::FILL | Gtk::SHRINK, Gtk::SHRINK);
  details_table->attach(this->skill_level[2], 0, 1, 4, 5,
      Gtk::FILL | Gtk::SHRINK, Gtk::SHRINK);
  details_table->attach(this->skill_level[3], 0, 1, 5, 6,
      Gtk::FILL | Gtk::SHRINK, Gtk::SHRINK);
  details_table->attach(this->skill_level[4], 0, 1, 6, 7,
      Gtk::FILL | Gtk::SHRINK, Gtk::EXPAND | Gtk::FILL);
  details_table->attach(*text_scwin, 1, 2, 0, 7,
      Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND);

  Gtk::ScrolledWindow* deps_scwin = MK_SCWIN;
  deps_scwin->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
  deps_scwin->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  deps_scwin->add(this->deps_view);

  /* Pack stuff together, create vpaned window. */
  Gtk::VBox* details_vbox = MK_VBOX;
  details_vbox->pack_start(*details_skill_box, false, false, 0);
  details_vbox->pack_start(*details_table, true, true, 0);

  Gtk::VPaned* vpaned = Gtk::manage(new Gtk::VPaned);
  vpaned->set_position(1);
  vpaned->pack1(*details_vbox, true, false);
  vpaned->pack2(*deps_scwin, true, false);

  this->set_border_width(5);
  this->pack_start(*vpaned, true, true, 0);

  /* Init some labels. */
  this->skill_group.set_text("No skill selected!");
  this->skill_name.set_text("");
  this->skill_primary.set_text("Primary: <not available>");
  this->skill_secondary.set_text("Secondary: <not available>");
  for (unsigned int i = 0; i < 5; ++i)
    this->skill_level[i].set_text("To level "
        + Helpers::get_string_from_uint(i) + ": 0d 0h 0m 0s");
  this->skill_desc_buffer->set_text("");

  this->tooltips.set_tip(*queue_skill_but, "Enqueues this skill");

  this->history.signal_skill_changed().connect(sigc::mem_fun
      (*this, &GtkSkillDetails::on_skill_changed));
  this->deps_view.signal_row_activated().connect
      (sigc::mem_fun(*this, &GtkSkillDetails::on_skill_selected));
}

/* ---------------------------------------------------------------- */

void
GtkSkillDetails::set_skill (ApiSkill const* skill)
{
  this->history.append_skill(skill);
}

/* ---------------------------------------------------------------- */

void
GtkSkillDetails::on_skill_changed (ApiSkill const* skill)
{
  /* Fill character invariant values. */
  ApiSkillTreePtr tree = ApiSkillTree::request();
  ApiSkillGroup const* group = tree->get_group_for_id(skill->group);

  this->skill_group.set_text("<b>" + group->name + "</b>");
  this->skill_name.set_text("<b>" + skill->name + " (rank "
      + Helpers::get_string_from_int(skill->rank) + ")</b>");
  this->skill_group.set_use_markup(true);
  this->skill_name.set_use_markup(true);

  this->skill_primary.set_text(Glib::ustring("Primary: ")
      + ApiSkillTree::get_attrib_name(skill->primary));
  this->skill_secondary.set_text(Glib::ustring("Secondary: ")
      + ApiSkillTree::get_attrib_name(skill->secondary));

  this->skill_desc_buffer->set_text(skill->desc);

  /* Fill character related values. */
  if (this->charsheet.get() == 0 || !this->charsheet->valid)
    return;

  ApiCharSheetSkill* cskill = this->charsheet->get_skill_for_id(skill->id);
  time_t timediff = 0;
  double spps = this->charsheet->get_sppm_for_skill(skill) / 60.0;

  for (unsigned int i = 0; i < 5; ++i)
  {
    Glib::ustring to_level_str = "To level "
        + Helpers::get_string_from_int(i + 1) + ": ";

    if (cskill == 0 || cskill->level < (int)i)
    {
      int start_sp = this->charsheet->calc_start_sp(i, skill->rank);
      int dest_sp = this->charsheet->calc_dest_sp(i, skill->rank);
      timediff += (time_t)((double)(dest_sp - start_sp) / spps);
      to_level_str += EveTime::get_string_for_timediff(timediff, true);
    }
    else if (cskill->level == (int)i)
    {
      int diff_sp = cskill->points_dest - cskill->points;
      timediff += (time_t)((double)diff_sp / spps);
      to_level_str += EveTime::get_string_for_timediff(timediff, true);
      if (cskill->completed != 0.0)
        to_level_str += " (" + Helpers::get_string_from_double
            (cskill->completed * 100.0, 0) + "%)";
    }

    this->skill_level[i].set_text(to_level_str);
  }

  this->deps_store->clear();
  this->recurse_append_skill_req(skill, this->deps_store->append(), 1);
  this->deps_view.expand_all();
}

/* ---------------------------------------------------------------- */

void
GtkSkillDetails::on_skill_selected (Gtk::TreeModel::Path const& path,
    Gtk::TreeViewColumn* col)
{
  col = 0;

  Gtk::TreeModel::iterator iter = this->deps_store->get_iter(path);
  ApiSkill const* skill = (*iter)[this->deps_cols.skill];

  if (skill == 0)
    return;

  this->set_skill(skill);
}

/* ---------------------------------------------------------------- */

void
GtkSkillDetails::recurse_append_skill_req (ApiSkill const* skill,
    Gtk::TreeModel::iterator slot, int level)
{
  (*slot)[this->deps_cols.name] = skill->name + " "
      + Helpers::get_roman_from_int(level);
  (*slot)[this->deps_cols.skill] = skill;

  ApiCharSheetSkill* cskill = this->charsheet->get_skill_for_id(skill->id);
  Glib::RefPtr<Gdk::Pixbuf> skill_icon;
  if (cskill == 0)
    skill_icon = ImageStore::skilldeps[0];
  else if (cskill->level >= level)
    skill_icon = ImageStore::skilldeps[2];
  else
    skill_icon = ImageStore::skilldeps[1];
  (*slot)[this->deps_cols.icon] = skill_icon;

  ApiSkillTreePtr tree = ApiSkillTree::request();
  for (unsigned int i = 0; i < skill->deps.size(); ++i)
  {
    int skill_id = skill->deps[i].first;
    int newlevel = skill->deps[i].second;
    ApiSkill const* newskill = tree->get_skill_for_id(skill_id);
    this->recurse_append_skill_req(newskill,
        this->deps_store->append(slot->children()), newlevel);
  }
}

/* ================================================================ */

GtkTrainingPlan::GtkTrainingPlan (void)
{
  this->add(*MK_LABEL("Test"));
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::append_skill (ApiSkill* skill)
{
  this->list.push_back(skill);
  this->update_plan();
}

/* ---------------------------------------------------------------- */

void
GtkTrainingPlan::update_plan (void)
{
}

/* ================================================================ */

GuiSkillPlanner::GuiSkillPlanner (void)
  : skill_store(Gtk::TreeStore::create(skill_cols)),
    skill_view(skill_store)
{
  this->skill_store->set_sort_column
      (this->skill_cols.name, Gtk::SORT_ASCENDING);

  Gtk::TreeViewColumn* skill_col_name = Gtk::manage(new Gtk::TreeViewColumn);
  skill_col_name->set_title("Name");
  skill_col_name->pack_start(this->skill_cols.icon, false);
  skill_col_name->pack_start(this->skill_cols.name, true);
  this->skill_view.append_column(*skill_col_name);
  this->skill_view.set_headers_visible(false);
  this->skill_view.get_selection()->set_mode(Gtk::SELECTION_SINGLE);

  Gtk::ScrolledWindow* skill_scwin = MK_SCWIN;
  skill_scwin->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  skill_scwin->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
  skill_scwin->add(this->skill_view);

  Gtk::Button* clear_filter_but = MK_BUT0;
  clear_filter_but->set_image(*MK_IMG(Gtk::Stock::CLEAR, Gtk::ICON_SIZE_MENU));
  clear_filter_but->set_relief(Gtk::RELIEF_NONE);

  Gtk::HBox* filter_box = MK_HBOX;
  filter_box->pack_start(*MK_LABEL("Filter:"), false, false, 0);
  filter_box->pack_start(this->filter_entry, true, true, 0);
  filter_box->pack_start(*clear_filter_but, false, false, 0);

  Gtk::VBox* skill_panechild = MK_VBOX;
  skill_panechild->pack_start(*filter_box, false, false, 0);
  skill_panechild->pack_start(*skill_scwin, true, true, 0);
  skill_panechild->set_size_request(250, -1);

  Gtk::Notebook* details_notebook = MK_NOTEBOOK;
  details_notebook->append_page(this->details_gui, "Skill details");
  //details_notebook->append_page(this->plan_gui, "Training plan");

  Gtk::Button* close_but = MK_BUT(Gtk::Stock::CLOSE);
  Gtk::HBox* button_hbox = MK_HBOX;
  button_hbox->pack_start(*MK_HSEP, true, true, 0);
  button_hbox->pack_start(*close_but, false, false, 0);

  Gtk::VBox* details_panechild = MK_VBOX;
  details_panechild->pack_start(*details_notebook, true, true, 0);
  details_panechild->pack_start(*button_hbox, false, false, 0);

  Gtk::HBox* header_box = MK_HBOX;
  header_box->pack_start(*MK_LABEL("Character:"), false, false, 0);
  header_box->pack_start(this->character_label, false, false, 0);
  header_box->pack_start(*MK_HSEP, true, true, 0);

  Gtk::HPaned* main_pane = MK_HPANED;
  main_pane->add1(*skill_panechild);
  main_pane->add2(*details_panechild);

  Gtk::VBox* main_vbox = MK_VBOX;
  main_vbox->set_border_width(5);
  main_vbox->pack_start(*header_box, false, false, 0);
  main_vbox->pack_start(*main_pane, true, true, 0);

  this->tooltips.set_tip(*clear_filter_but, "Clears the filter");
  this->tooltips.set_tip(this->filter_entry, "Filtering is case-sensitive");

  this->skill_view.get_selection()->signal_changed().connect
      (sigc::mem_fun(*this, &GuiSkillPlanner::skill_selected));
  this->skill_view.signal_row_activated().connect
      (sigc::mem_fun(*this, &GuiSkillPlanner::skill_row_activated));
  close_but->signal_clicked().connect(sigc::mem_fun(*this, &WinBase::close));
  this->filter_entry.signal_activate().connect(sigc::mem_fun
      (*this, &GuiSkillPlanner::fill_skill_store));
  clear_filter_but->signal_clicked().connect(sigc::mem_fun
      (*this, &GuiSkillPlanner::clear_filter));

  this->add(*main_vbox);
  this->set_title("Skill browser - GtkEveMon");
  this->set_icon(ImageStore::applogo);
  this->set_default_size(800, 550);
  this->show_all();
}

/* ---------------------------------------------------------------- */

GuiSkillPlanner::~GuiSkillPlanner (void)
{
}

/* ---------------------------------------------------------------- */

void
GuiSkillPlanner::set_character (ApiCharSheetPtr sheet)
{
  this->charsheet = sheet;
  this->details_gui.set_character(sheet);
  this->fill_skill_store();
  this->character_label.set_text(this->charsheet->name);
  this->set_title(this->charsheet->name + " - GtkEveMon");
}

/* ---------------------------------------------------------------- */

void
GuiSkillPlanner::fill_skill_store (void)
{
  this->skill_store->clear();
  Glib::ustring filter = this->filter_entry.get_text();

  ApiSkillTreePtr tree = ApiSkillTree::request();
  ApiSkillMap& skills = tree->skills;
  ApiSkillGroupMap& groups = tree->groups;

  typedef Gtk::TreeModel::iterator GtkTreeModelIter;
  typedef std::map<int, std::pair<GtkTreeModelIter, int> > SkillGroupsMap;
  SkillGroupsMap skill_group_iters;

  /* Append all skill groups to the store. */
  for (ApiSkillGroupMap::iterator iter = groups.begin();
      iter != groups.end(); iter++)
  {
    Gtk::TreeModel::iterator siter = this->skill_store->append();
    (*siter)[this->skill_cols.name] = iter->second.name;
    (*siter)[this->skill_cols.icon] = ImageStore::skillicons[0];
    (*siter)[this->skill_cols.skill] = 0;
    skill_group_iters.insert(std::make_pair
        (iter->first, std::make_pair(siter, 0)));
  }

  /* Append all skills to the skill groups. */
  for (ApiSkillMap::iterator iter = skills.begin();
      iter != skills.end(); iter++)
  {
    ApiSkill& skill = iter->second;

    /* Apply filter. */
    if (Glib::ustring(skill.name).casefold()
        .find(filter.casefold()) == Glib::ustring::npos)
      continue;

    SkillGroupsMap::iterator giter = skill_group_iters.find(skill.group);
    if (giter == skill_group_iters.end())
    {
      std::cout << "Error appending skill, unknown group!" << std::endl;
      continue;
    }

    Gtk::TreeModel::iterator siter = this->skill_store->append
        (giter->second.first->children());
    (*siter)[this->skill_cols.name] = skill.name + " ("
        + Helpers::get_string_from_int(skill.rank) + ")";
    (*siter)[this->skill_cols.skill] = &skill;

    ApiCharSheetSkill* cskill = this->charsheet->get_skill_for_id(skill.id);
    Glib::RefPtr<Gdk::Pixbuf> skill_icon;

    if (cskill == 0)
    {
      if (this->have_prerequisites_for_skill(&skill))
        skill_icon = ImageStore::skillstatus[1];
      else
        skill_icon = ImageStore::skillstatus[0];

    }
    else
    {
      switch (cskill->level)
      {
        case 0: skill_icon = ImageStore::skillstatus[2]; break;
        case 1: skill_icon = ImageStore::skillstatus[3]; break;
        case 2: skill_icon = ImageStore::skillstatus[4]; break;
        case 3: skill_icon = ImageStore::skillstatus[5]; break;
        case 4: skill_icon = ImageStore::skillstatus[6]; break;
        case 5: skill_icon = ImageStore::skillstatus[7]; break;
        default: skill_icon = ImageStore::skillstatus[0]; break;
      }
    }

    (*siter)[this->skill_cols.icon] = skill_icon;

    giter->second.second += 1;
  }

  /* Remove empty groups (due to filtering). */
  for (SkillGroupsMap::iterator iter = skill_group_iters.begin();
      iter != skill_group_iters.end(); iter++)
  {
    if (iter->second.second == 0)
      this->skill_store->erase(iter->second.first);
  }

  if (!filter.empty())
    this->skill_view.expand_all();
}

/* ---------------------------------------------------------------- */

void
GuiSkillPlanner::skill_selected (void)
{
  if (this->skill_view.get_selection()->get_selected_rows().empty())
    return;

  Gtk::TreeModel::iterator iter = this->skill_view.get_selection
      ()->get_selected();

  ApiSkill const* skill = (*iter)[this->skill_cols.skill];
  if (skill == 0)
    return;

  this->details_gui.set_skill(skill);
}

/* ---------------------------------------------------------------- */

void
GuiSkillPlanner::skill_row_activated (Gtk::TreeModel::Path const& path,
    Gtk::TreeViewColumn* col)
{
  col = 0;

  Gtk::TreeModel::iterator iter = this->skill_store->get_iter(path);
  ApiSkill const* skill = (*iter)[this->skill_cols.skill];

  if (skill != 0)
  {
    this->details_gui.set_skill(skill);
  }
  else
  {
    if (this->skill_view.row_expanded(path))
      this->skill_view.collapse_row(path);
    else
      this->skill_view.expand_row(path, true);
  }
}

/* ---------------------------------------------------------------- */

void
GuiSkillPlanner::clear_filter (void)
{
  this->filter_entry.set_text("");
  this->fill_skill_store();
}

/* ---------------------------------------------------------------- */

bool
GuiSkillPlanner::have_prerequisites_for_skill (ApiSkill const* skill)
{
  ApiSkillTreePtr tree = ApiSkillTree::request();
  for (unsigned int i = 0; i < skill->deps.size(); ++i)
  {
    int depskill_id = skill->deps[i].first;
    int depskill_level = skill->deps[i].second;

    ApiCharSheetSkill* cskill = this->charsheet->get_skill_for_id(depskill_id);
    if (cskill == 0 || cskill->level < depskill_level)
      return false;
  }

  return true;
}
