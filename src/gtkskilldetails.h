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

#ifndef GTK_SKILL_DETAILS_HEADER
#define GTK_SKILL_DETAILS_HEADER

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/tooltips.h>

#include "apiskilltree.h"
#include "apicharsheet.h"
#include "gtkplannerbase.h"

/* The maximum history size. */
#define HISTORY_MAX_SIZE 9

class GtkSkillHistory : public Gtk::HBox
{
  private:
    std::vector<ApiSkill const*> history;
    unsigned int history_pos;
    sigc::signal<void, ApiSkill const*> sig_skill_changed;

    Gtk::Button back_but;
    Gtk::Button next_but;
    Gtk::Label position_label;

    void update_sensitive (void);
    void update_pos_label (void);
    void back_clicked (void);
    void next_clicked (void);

  public:
    GtkSkillHistory (void);

    void append_skill (ApiSkill const* skill);
    sigc::signal<void, ApiSkill const*>& signal_skill_changed (void);
};

/* ---------------------------------------------------------------- */

typedef sigc::signal<void, ApiSkill const*, int> SignalPlanningRequested;

class GtkSkillDetails : public Gtk::VBox
{
  private:
    ApiCharSheetPtr charsheet;

    /* Dependency list. */
    GuiPlannerSkillCols deps_cols;
    Glib::RefPtr<Gtk::TreeStore> deps_store;
    GtkSkillListView deps_view;

    /* Skill details. */
    Gtk::Label skill_group;
    Gtk::Label skill_name;
    Gtk::Label skill_primary;
    Gtk::Label skill_secondary;
    Gtk::Label skill_level[5];
    Glib::RefPtr<Gtk::TextBuffer> skill_desc_buffer;

    GtkSkillHistory history;
    Gtk::Tooltips tooltips;

    SignalPlanningRequested sig_planning_requested;

    void recurse_append_skill_req (ApiSkill const* skill,
        Gtk::TreeModel::iterator slot, int level);
    double get_spps_for_skill (ApiSkill const* skill);
    void on_skill_changed (ApiSkill const* skill);
    void on_skill_selected (Gtk::TreeModel::Path const& path,
        Gtk::TreeViewColumn* col);
    void on_view_button_pressed (GdkEventButton* event);

  public:
    GtkSkillDetails (void);

    void set_skill (ApiSkill const* skill);
    void set_character (ApiCharSheetPtr character);

    SignalPlanningRequested& signal_planning_requested (void);
};

/* ---------------------------------------------------------------- */

inline sigc::signal<void, ApiSkill const*>&
GtkSkillHistory::signal_skill_changed (void)
{
  return this->sig_skill_changed;
}

inline void
GtkSkillDetails::set_character (ApiCharSheetPtr character)
{
  this->charsheet = character;
}

inline SignalPlanningRequested&
GtkSkillDetails::signal_planning_requested (void)
{
  return sig_planning_requested;
}

#endif /* GTK_SKILL_DETAILS_HEADER */
