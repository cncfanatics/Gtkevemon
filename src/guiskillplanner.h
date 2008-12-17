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

#ifndef GUI_SKILL_PLANNER_HEADER
#define GUI_SKILL_PLANNER_HEADER

#include <gtkmm/treestore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/entry.h>
#include <gtkmm/tooltips.h>
#include <gtkmm/notebook.h>

#include "gtkplannerbase.h"
#include "gtkskilldetails.h"
#include "gtktrainingplan.h"
#include "apiskilltree.h"
#include "apicharsheet.h"
#include "apiintraining.h"
#include "winbase.h"

class GuiSkillPlanner : public WinBase
{
  private:
    GtkSkillDetails details_gui;
    GtkTrainingPlan plan_gui;

    /* Character stuff. */
    ApiCharSheetPtr charsheet;
    ApiInTrainingPtr training;

    /* Skill list. */
    GuiPlannerSkillCols skill_cols;
    Glib::RefPtr<Gtk::TreeStore> skill_store;
    GtkSkillListView skill_view;

    /* Misc. */
    Gtk::Notebook notebook;
    Gtk::Tooltips tooltips;

    /* Filters */
    Gtk::Entry filter_entry;
    Gtk::CheckButton filter_unknown;
    Gtk::CheckButton filter_trainable;
    Gtk::CheckButton filter_inchoate;
    Gtk::CheckButton filter_trained;

    void fill_skill_store (void);
    void skill_selected (void);
    void clear_filter (void);
    void skill_row_activated (Gtk::TreeModel::Path const& path,
        Gtk::TreeViewColumn* col);
    bool have_prerequisites_for_skill (ApiSkill const* skill);
    void on_view_button_pressed (GdkEventButton* event);
    void on_planning_requested (ApiSkill const* skill, int level);
    bool on_gtkmain_quit (void);

  public:
    GuiSkillPlanner (void);
    ~GuiSkillPlanner (void);

    void set_character (ApiCharSheetPtr sheet);
    void set_training (ApiInTrainingPtr training);
    void set_skill (ApiSkill const* skill);
};

#endif /* GUI_SKILL_PLANNER_HEADER */
