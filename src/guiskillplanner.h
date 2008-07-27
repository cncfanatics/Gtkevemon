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
#include <gtkmm/textview.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/entry.h>
#include <gtkmm/tooltips.h>

#include "apiskilltree.h"
#include "apicharsheet.h"
#include "winbase.h"

class GuiPlannerSkillCols : public Gtk::TreeModel::ColumnRecord
{
  public:
    GuiPlannerSkillCols (void);

    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > icon;
    Gtk::TreeModelColumn<ApiSkill const*> skill;
};

/* ---------------------------------------------------------------- */

class GuiSkillPlanner : public WinBase
{
  private:
    ApiCharSheetPtr charsheet;
    Gtk::Label character_label;

    /* Skill list. */
    GuiPlannerSkillCols skill_cols;
    Glib::RefPtr<Gtk::TreeStore> skill_store;
    Gtk::TreeView skill_view;

    /* Dependency list. */
    GuiPlannerSkillCols deps_cols;
    Glib::RefPtr<Gtk::TreeStore> deps_store;
    Gtk::TreeView deps_view;

    /* Skill details. */
    Gtk::Label skill_group;
    Gtk::Label skill_name;
    Gtk::Label skill_primary;
    Gtk::Label skill_secondary;
    Gtk::Label skill_level[5];
    Glib::RefPtr<Gtk::TextBuffer> skill_desc_buffer;

    /* Misc. */
    Gtk::Entry filter_entry;
    Gtk::Tooltips tooltips;

    void fill_skill_store (void);
    void skill_selected (void);
    void clear_filter (void);
    void recurse_append_skill_req (ApiSkill const* skill,
        Gtk::TreeModel::iterator slot, int level);
    void skill_row_activated (Gtk::TreeModel::Path const& path,
        Gtk::TreeViewColumn* col);
    double get_spps_for_skill (ApiSkill const* skill);
    bool have_prerequisites_for_skill (ApiSkill const* skill);

  public:
    GuiSkillPlanner (void);
    ~GuiSkillPlanner (void);

    void set_character (ApiCharSheetPtr sheet);
};

/* ---------------------------------------------------------------- */

inline
GuiPlannerSkillCols::GuiPlannerSkillCols (void)
{
  this->add(this->name);
  this->add(this->icon);
  this->add(this->skill);
}

#endif /* GUI_SKILL_PLANNER_HEADER */
