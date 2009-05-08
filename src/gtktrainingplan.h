/*
 * This file is part of GtkEveMon.
 *
 * GtkEveMon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with GtkEveMon. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GTK_TRAINING_PLAN
#define GTK_TRAINING_PLAN

#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/box.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>

#include "gtkportrait.h"
#include "gtkcolumnsbase.h"
#include "config.h"
#include "gtkconfwidgets.h"
#include "apiskilltree.h"
#include "apiintraining.h"
#include "apicharsheet.h"

/* Update the time values for skills this milli seconds. */
#define PLANNER_SKILL_TIME_UPDATE 10000

enum GtkSkillIcon
{
  SKILL_STATUS_TRAINED,
  SKILL_STATUS_TRAINING,
  SKILL_STATUS_TRAINABLE,
  SKILL_STATUS_UNTRAINABLE,
  SKILL_STATUS_MISSING_DEPS
};

struct GtkSkillInfo
{
  ApiSkill const* skill;
  bool is_objective;
  int plan_level;

  int start_sp;
  int dest_sp;
  time_t train_duration;
  time_t skill_duration;
  time_t finish_time;
  time_t start_time;
  double completed;
  int spph;
  GtkSkillIcon skill_icon;
};

class GtkSkillList : public std::vector<GtkSkillInfo>
{
  private:
    ApiCharSheetPtr charsheet;
    ApiInTrainingPtr training;

  protected:
    void append_skill (ApiSkill const* skill, int level, bool objective);
    void apply_attributes (ApiSkill const* skill, ApiCharAttribs& attribs,
        int& learning_level);

  public:
    GtkSkillList (void);
    void set_character (ApiCharSheetPtr charsheet);
    void set_training (ApiInTrainingPtr training);

    ApiCharSheetPtr get_character (void);
    ApiInTrainingPtr get_training (void);

    void append_skill (ApiSkill const* skill, int level);
    void move_skill (unsigned int from, unsigned int to);
    //void fix_skill (unsigned int index);
    void insert_skill (unsigned int pos, GtkSkillInfo const& info);
    void release_skill (unsigned int index);
    void delete_skill (unsigned int index);
    void cleanup_skills (void);
    bool has_plan_dep_skills (unsigned int index);
    bool has_char_dep_skills (ApiSkill const* skill, int level);
    bool has_char_skill (ApiSkill const* skill, int level);
    bool has_plan_skill (ApiSkill const* skill, int level,
        bool make_objective = false);
    bool is_dependency (unsigned int index);

    /* Calculate all details for the skill plan. If attributes and
     * the learning level are specified, these are used instead
     * of the character ones. "use_active_spph" specifies if the SP/h
     * for the skill in training is taken from the training sheet. */
    void calc_details (bool use_active_spph = true);
    void calc_details (ApiCharAttribs& attribs, int learning_level,
        bool use_active_spph = true);
    //void simulate_select (unsigned int index);
};

/* ---------------------------------------------------------------- */

class GtkTreeModelColumns : public Gtk::TreeModel::ColumnRecord
{
  public:
    Gtk::TreeModelColumn<unsigned int> skill_index;
    Gtk::TreeModelColumn<bool> objective;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > skill_icon;
    Gtk::TreeModelColumn<Glib::ustring> skill_name;
    Gtk::TreeModelColumn<Glib::ustring> train_duration;
    Gtk::TreeModelColumn<Glib::ustring> skill_duration;
    Gtk::TreeModelColumn<Glib::ustring> completed;
    Gtk::TreeModelColumn<Glib::ustring> attributes;
    Gtk::TreeModelColumn<Glib::ustring> est_start;
    Gtk::TreeModelColumn<Glib::ustring> est_finish;
    Gtk::TreeModelColumn<int> spph;

    GtkTreeModelColumns (void);
};

/* ---------------------------------------------------------------- */

class GtkTreeViewColumns : public GtkColumnsBase
{
  public:
    Gtk::TreeView::Column objective;
    Gtk::TreeView::Column skill_name;
    Gtk::TreeView::Column train_duration;
    Gtk::TreeView::Column skill_duration;
    Gtk::TreeView::Column completed;
    Gtk::TreeView::Column attributes;
    Gtk::TreeView::Column est_start;
    Gtk::TreeView::Column est_finish;
    Gtk::TreeView::Column spph;

    GtkTreeViewColumns (Gtk::TreeView* view, GtkTreeModelColumns* cols);
};

/* ---------------------------------------------------------------- */

class GtkTrainingPlan : public Gtk::VBox
{
  private:
    ApiCharSheetPtr charsheet;
    ApiInTrainingPtr training;
    GtkSkillList skills;

    GtkConfSectionSelection plan_selection;
    ConfSectionPtr plan_section;

    GtkPortrait portrait;
    Gtk::Button delete_plan_but;
    Gtk::Button create_plan_but;
    Gtk::Button rename_plan_but;
    Gtk::Button clean_plan_but;
    Gtk::Button column_conf_but;
    Gtk::Button export_plan_but;
    Gtk::Button import_plan_but;
    Gtk::Button optimize_att_but;
    Gtk::Label total_time;

    GtkTreeModelColumns cols;
    Glib::RefPtr<Gtk::ListStore> liststore;
    Gtk::TreeView treeview;
    GtkTreeViewColumns viewcols;
    bool updating_liststore;
    int reorder_new_index;

    sigc::signal<void, ApiSkill const*> sig_skill_activated;

  protected:
    void update_plan (bool rebuild);

    void init_from_config (void);
    void store_to_config (void);
    void skill_plan_changed (ConfSectionPtr section);
    void save_current_plan (void);
    void load_current_plan (void);
    void on_create_skill_plan (void);
    void on_delete_skill_plan (void);
    void on_rename_skill_plan (void);
    void on_cleanup_skill_plan (void);
    void on_objective_toggled (Glib::ustring const& path);
    void on_export_plan (void);
    void on_import_plan (void);
    void on_optimize_att (void);

    void on_row_inserted (Gtk::TreePath const& path,
        Gtk::TreeModel::iterator const& iter);
    void on_row_deleted (Gtk::TreePath const& path);
    void on_row_activated (Gtk::TreeModel::Path const& path,
        Gtk::TreeViewColumn* column);
    bool on_update_skill_time (void);
    bool on_query_skillview_tooltip (int x, int y, bool key,
        Glib::RefPtr<Gtk::Tooltip> const& tooltip);

  public:
    GtkTrainingPlan (void);
    ~GtkTrainingPlan (void);

    void set_character (ApiCharSheetPtr character);
    void set_training (ApiInTrainingPtr training);

    void append_skill (ApiSkill const* skill, int level);

    sigc::signal<void, ApiSkill const*>& signal_skill_activated (void);
};

/* ---------------------------------------------------------------- */

inline void
GtkSkillList::set_character (ApiCharSheetPtr charsheet)
{
  this->charsheet = charsheet;
}

inline void
GtkSkillList::set_training (ApiInTrainingPtr training)
{
  this->training = training;
}

inline ApiCharSheetPtr
GtkSkillList::get_character (void)
{
  return this->charsheet;
}

inline ApiInTrainingPtr
GtkSkillList::get_training (void)
{
  return this->training;
}

inline void
GtkSkillList::append_skill (ApiSkill const* skill, int level)
{
  this->append_skill(skill, level, true);
}

inline sigc::signal<void, ApiSkill const*>&
GtkTrainingPlan::signal_skill_activated (void)
{
  return this->sig_skill_activated;
}

#endif /* GTK_TRAINING_PLAN */
