#include <iostream>
#include <gtkmm/main.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/stock.h>
#include <gtkmm/image.h>
#include <gtkmm/paned.h>
#include <gtkmm/separator.h>
#include <gtkmm/scrolledwindow.h>

#include "helpers.h"
#include "imagestore.h"
#include "gtkportrait.h"
#include "gtkdefines.h"
#include "guiskillplanner.h"

GuiSkillPlanner::GuiSkillPlanner (void)
  : skill_store(Gtk::TreeStore::create(skill_cols)),
    skill_view(skill_store),
    filter_unknown("Only unknown"),
    filter_trainable("Only trainable")
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

  Gtk::HBox* filter_cb_box = MK_HBOX;
  filter_cb_box->pack_start(this->filter_unknown, false, false, 0);
  filter_cb_box->pack_start(this->filter_trainable, false, false, 0);

  Gtk::VBox* skill_panechild = MK_VBOX;
  skill_panechild->pack_start(*filter_box, false, false, 0);
  skill_panechild->pack_start(*filter_cb_box, false, false, 0);
  skill_panechild->pack_start(*skill_scwin, true, true, 0);
  skill_panechild->set_size_request(250, -1);

  this->notebook.append_page(this->plan_gui, "Training plan");
  this->notebook.append_page(this->details_gui, "Skill details");

  Gtk::Button* close_but = MK_BUT(Gtk::Stock::CLOSE);
  Gtk::HBox* button_hbox = MK_HBOX;
  button_hbox->pack_start(*MK_HSEP, true, true, 0);
  button_hbox->pack_start(*close_but, false, false, 0);

  Gtk::VBox* details_panechild = MK_VBOX;
  details_panechild->pack_start(this->notebook, true, true, 0);
  details_panechild->pack_start(*button_hbox, false, false, 0);

  /*
  Gtk::HBox* header_box = MK_HBOX;
  header_box->pack_start(*MK_LABEL("Character:"), false, false, 0);
  header_box->pack_start(this->character_label, false, false, 0);
  header_box->pack_start(*MK_HSEP, true, true, 0);
  */

  Gtk::HPaned* main_pane = MK_HPANED;
  main_pane->add1(*skill_panechild);
  main_pane->add2(*details_panechild);

  Gtk::VBox* main_vbox = MK_VBOX;
  main_vbox->set_border_width(5);
  //main_vbox->pack_start(*header_box, false, false, 0);
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
  this->filter_unknown.signal_clicked().connect(sigc::mem_fun
      (*this, &GuiSkillPlanner::fill_skill_store));
  this->filter_trainable.signal_clicked().connect(sigc::mem_fun
      (*this, &GuiSkillPlanner::fill_skill_store));
  clear_filter_but->signal_clicked().connect(sigc::mem_fun
      (*this, &GuiSkillPlanner::clear_filter));
  this->plan_gui.signal_skill_activated().connect(sigc::mem_fun
      (*this, &GuiSkillPlanner::set_skill));
  this->skill_view.signal_button_press_myevent().connect(sigc::mem_fun
      (*this, &GuiSkillPlanner::on_view_button_pressed));
  this->details_gui.signal_planning_requested().connect(sigc::mem_fun
      (*this, &GuiSkillPlanner::on_planning_requested));
  Gtk::Main::signal_quit().connect(sigc::mem_fun
      (*this, &GuiSkillPlanner::on_gtkmain_quit));

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
  this->plan_gui.set_character(sheet);
  this->fill_skill_store();
  //this->character_label.set_text(this->charsheet->name);
  this->set_title(this->charsheet->name + " - GtkEveMon");
}

/* ---------------------------------------------------------------- */

void
GuiSkillPlanner::set_training (ApiInTrainingPtr training)
{
  this->training = training;
  this->plan_gui.set_training(training);
}

/* ---------------------------------------------------------------- */

void
GuiSkillPlanner::set_skill (ApiSkill const* skill)
{
  this->notebook.set_current_page(1);
  this->details_gui.set_skill(skill);
}

/* ---------------------------------------------------------------- */

void
GuiSkillPlanner::fill_skill_store (void)
{
  this->skill_store->clear();
  Glib::ustring filter = this->filter_entry.get_text();
  bool only_unknown = this->filter_unknown.get_active();
  bool only_trainable = this->filter_trainable.get_active();

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

    /* Apply string filter. */
    if (Glib::ustring(skill.name).casefold()
        .find(filter.casefold()) == Glib::ustring::npos)
      continue;

    SkillGroupsMap::iterator giter = skill_group_iters.find(skill.group);
    if (giter == skill_group_iters.end())
    {
      std::cout << "Error appending skill, unknown group!" << std::endl;
      continue;
    }

    ApiCharSheetSkill* cskill = this->charsheet->get_skill_for_id(skill.id);
    Glib::RefPtr<Gdk::Pixbuf> skill_icon;

    if (cskill == 0)
    {
      if (this->have_prerequisites_for_skill(&skill))
        skill_icon = ImageStore::skillstatus[1];
      else
      {
        if (only_trainable)
          continue;
        skill_icon = ImageStore::skillstatus[0];
      }

    }
    else
    {
      if (only_unknown)
        continue;

      switch (cskill->level)
      {
        case 0: skill_icon = ImageStore::skillstatus[2]; break;
        case 1: skill_icon = ImageStore::skillstatus[3]; break;
        case 2: skill_icon = ImageStore::skillstatus[4]; break;
        case 3: skill_icon = ImageStore::skillstatus[5]; break;
        case 4: skill_icon = ImageStore::skillstatus[6]; break;
        case 5:
          skill_icon = ImageStore::skillstatus[7];
          if (only_trainable)
            continue;
          break;
        default: skill_icon = ImageStore::skillstatus[0]; break;
      }
    }

    /* Finally append the skill. */
    Gtk::TreeModel::iterator siter = this->skill_store->append
        (giter->second.first->children());
    (*siter)[this->skill_cols.name] = skill.name + " ("
        + Helpers::get_string_from_int(skill.rank) + ")";
    (*siter)[this->skill_cols.skill] = &skill;

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

  if (!filter.empty() || only_unknown || only_trainable)
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
    this->set_skill(skill);
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

/* ---------------------------------------------------------------- */

void
GuiSkillPlanner::on_view_button_pressed (GdkEventButton* event)
{
  if (event->type != GDK_BUTTON_PRESS || event->button != 3)
    return;

  Glib::RefPtr<Gtk::TreeView::Selection> selection
      = this->skill_view.get_selection();

  if (selection->count_selected_rows() != 1)
    return;

  Gtk::TreeModel::iterator iter = selection->get_selected();
  ApiSkill const* skill = (*iter)[this->skill_cols.skill];
  if (skill == 0)
    return;

  GtkSkillContextMenu* context = Gtk::manage(new GtkSkillContextMenu);
  context->set_skill(skill, this->charsheet->get_level_for_skill(skill->id));
  context->popup(event->button, event->time);
  context->signal_planning_requested().connect(sigc::mem_fun
      (*this, &GuiSkillPlanner::on_planning_requested));

  return;
}

/* ---------------------------------------------------------------- */

void
GuiSkillPlanner::on_planning_requested (ApiSkill const* skill, int level)
{
  this->notebook.set_current_page(0);
  this->plan_gui.append_skill(skill, level);
}

/* ---------------------------------------------------------------- */

bool
GuiSkillPlanner::on_gtkmain_quit (void)
{
  this->close();
  return false;
}
