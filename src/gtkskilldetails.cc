#include <gtkmm/image.h>
#include <gtkmm/stock.h>
#include <gtkmm/textview.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/paned.h>
#include <gtkmm/table.h>
#include "helpers.h"
#include "evetime.h"
#include "imagestore.h"
#include "gtkdefines.h"
#include "gtkhelpers.h"
#include "gtkskilldetails.h"

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
  this->skill_group.set_alignment(Gtk::ALIGN_LEFT);
  this->skill_name.set_alignment(Gtk::ALIGN_LEFT);
  this->skill_primary.set_alignment(Gtk::ALIGN_LEFT);
  this->skill_secondary.set_alignment(Gtk::ALIGN_LEFT);
  for (unsigned int i = 0; i < 5; ++i)
  {
    this->skill_level[i].set_alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_TOP);
    this->skill_level[i].set_selectable(true);
  }

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

  Gtk::Label* primary_label = MK_LABEL("Primary:");
  Gtk::Label* secondary_label = MK_LABEL("Secondary:");
  primary_label->set_alignment(Gtk::ALIGN_LEFT);
  secondary_label->set_alignment(Gtk::ALIGN_LEFT);
  Gtk::Label* to_level_label[5];
  for (unsigned int i = 0; i < 5; ++i)
  {
    to_level_label[i] = Gtk::manage(new Gtk::Label);
    to_level_label[i]->set_text("To level "
        + Helpers::get_string_from_int(i + 1) + ":");
    to_level_label[i]->set_alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_TOP);
  }

  Gtk::Table* details_table = Gtk::manage(new Gtk::Table(7, 3, false));
  details_table->set_col_spacings(5);
  details_table->attach(*primary_label, 0, 1, 0, 1,
      Gtk::FILL | Gtk::SHRINK, Gtk::SHRINK);
  details_table->attach(*secondary_label, 0, 1, 1, 2,
      Gtk::FILL | Gtk::SHRINK, Gtk::SHRINK);
  details_table->attach(*to_level_label[0], 0, 1, 2, 3,
      Gtk::FILL | Gtk::SHRINK, Gtk::SHRINK);
  details_table->attach(*to_level_label[1], 0, 1, 3, 4,
      Gtk::FILL | Gtk::SHRINK, Gtk::SHRINK);
  details_table->attach(*to_level_label[2], 0, 1, 4, 5,
      Gtk::FILL | Gtk::SHRINK, Gtk::SHRINK);
  details_table->attach(*to_level_label[3], 0, 1, 5, 6,
      Gtk::FILL | Gtk::SHRINK, Gtk::SHRINK);
  details_table->attach(*to_level_label[4], 0, 1, 6, 7,
      Gtk::FILL | Gtk::SHRINK, Gtk::EXPAND | Gtk::FILL);

  details_table->attach(this->skill_primary, 1, 2, 0, 1,
      Gtk::FILL | Gtk::SHRINK, Gtk::SHRINK);
  details_table->attach(this->skill_secondary, 1, 2, 1, 2,
      Gtk::FILL | Gtk::SHRINK, Gtk::SHRINK);
  details_table->attach(this->skill_level[0], 1, 2, 2, 3,
      Gtk::FILL | Gtk::SHRINK, Gtk::SHRINK);
  details_table->attach(this->skill_level[1], 1, 2, 3, 4,
      Gtk::FILL | Gtk::SHRINK, Gtk::SHRINK);
  details_table->attach(this->skill_level[2], 1, 2, 4, 5,
      Gtk::FILL | Gtk::SHRINK, Gtk::SHRINK);
  details_table->attach(this->skill_level[3], 1, 2, 5, 6,
      Gtk::FILL | Gtk::SHRINK, Gtk::SHRINK);
  details_table->attach(this->skill_level[4], 1, 2, 6, 7,
      Gtk::FILL | Gtk::SHRINK, Gtk::EXPAND | Gtk::FILL);

  details_table->attach(*text_scwin, 2, 3, 0, 7,
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
  this->skill_primary.set_text("<not available>");
  this->skill_secondary.set_text("<not available>");
  for (unsigned int i = 0; i < 5; ++i)
    this->skill_level[i].set_text("0d 0h");
  this->skill_desc_buffer->set_text("");

  this->tooltips.set_tip(*queue_skill_but, "Enqueues this skill");

  this->history.signal_skill_changed().connect(sigc::mem_fun
      (*this, &GtkSkillDetails::on_skill_changed));
  this->deps_view.signal_row_activated().connect
      (sigc::mem_fun(*this, &GtkSkillDetails::on_skill_selected));
  this->deps_view.signal_button_press_myevent().connect(sigc::mem_fun
      (*this, &GtkSkillDetails::on_view_button_pressed));
  this->deps_view.signal_query_tooltip().connect(sigc::mem_fun
      (*this, &GtkSkillDetails::on_query_skillview_tooltip));
  this->deps_view.set_has_tooltip(true);
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

  this->skill_primary.set_text
      (ApiSkillTree::get_attrib_name(skill->primary));
  this->skill_secondary.set_text
      (ApiSkillTree::get_attrib_name(skill->secondary));

  this->skill_desc_buffer->set_text(skill->desc);

  /* Fill character related values. */
  if (this->charsheet.get() == 0 || !this->charsheet->valid)
    return;

  ApiCharSheetSkill* cskill = this->charsheet->get_skill_for_id(skill->id);
  time_t timediff = 0;
  double spps = this->charsheet->get_sppm_for_skill(skill) / 60.0;

  for (unsigned int i = 0; i < 5; ++i)
  {
    Glib::ustring to_level_str;
    if (cskill == 0 || cskill->level < (int)i)
    {
      int start_sp = this->charsheet->calc_start_sp(i, skill->rank);
      int dest_sp = this->charsheet->calc_dest_sp(i, skill->rank);
      timediff += (time_t)((double)(dest_sp - start_sp) / spps);
      to_level_str = EveTime::get_string_for_timediff(timediff, true);
    }
    else if (cskill->level == (int)i)
    {
      int diff_sp = cskill->points_dest - cskill->points;
      timediff += (time_t)((double)diff_sp / spps);
      to_level_str = EveTime::get_string_for_timediff(timediff, true);
      if (cskill->completed != 0.0)
        to_level_str += " (" + Helpers::get_string_from_double
            (cskill->completed * 100.0, 0) + "%)";
    }
    else
    {
      to_level_str = "--";
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

/* ---------------------------------------------------------------- */

void
GtkSkillDetails::on_view_button_pressed (GdkEventButton* event)
{
  if (event->type != GDK_BUTTON_PRESS || event->button != 3)
    return;

  Glib::RefPtr<Gtk::TreeView::Selection> selection
      = this->deps_view.get_selection();

  if (selection->count_selected_rows() != 1)
    return;

  Gtk::TreeModel::iterator iter = selection->get_selected();
  ApiSkill const* skill = (*iter)[this->deps_cols.skill];
  if (skill == 0)
    return;

  GtkSkillContextMenu* context = Gtk::manage(new GtkSkillContextMenu);
  context->set_skill(skill, this->charsheet->get_level_for_skill(skill->id));
  context->popup(event->button, event->time);
  context->signal_planning_requested().connect(sigc::mem_fun
      (this->sig_planning_requested, &SignalPlanningRequested::emit));

  return;
}

/* ---------------------------------------------------------------- */

bool
GtkSkillDetails::on_query_skillview_tooltip (int x, int y, bool key,
    Glib::RefPtr<Gtk::Tooltip> const& tooltip)
{
  key = false;

  Gtk::TreeModel::Path path;
  Gtk::TreeViewDropPosition pos;

  bool exists = this->deps_view.get_dest_row_at_pos(x, y, path, pos);

  if (!exists)
    return false;

  Gtk::TreeIter iter = this->deps_store->get_iter(path);
  ApiSkill const* skill = (*iter)[this->deps_cols.skill];

  if (skill == 0)
    return false;

  GtkHelpers::create_tooltip(tooltip, skill);
  return true;
}
