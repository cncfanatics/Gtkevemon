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

#ifndef GTK_PLANNER_BASE_HEADER
#define GTK_PLANNER_BASE_HEADER

#include <gtkmm/treeview.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/menu.h>
#include <gdkmm/pixbuf.h>

#include "apiskilltree.h"
#include "apicerttree.h"

typedef sigc::signal<void, ApiSkill const*, int> SignalPlanningRequested;
typedef sigc::signal<void, ApiElement const*> SignalApiElementSelected;
typedef sigc::signal<void, ApiElement const*> SignalApiElementActivated;

/* ---------------------------------------------------------------- */

class GtkSkillContextMenu : public Gtk::Menu
{
  private:
    ApiSkill const* skill;
    sigc::signal<void, ApiSkill const*, int> sig_planning_requested;

  protected:
    void on_training_requested (int level);
    void delete_me (void);

  public:
    GtkSkillContextMenu (void);
    void set_skill (ApiSkill const* skill, int min_level);
    sigc::signal<void, ApiSkill const*, int>& signal_planning_requested (void);
};

/* ---------------------------------------------------------------- */

/* A wrapper around TreeView to handle context menu requests. */
class GtkListViewHelper : public Gtk::TreeView
{
  private:
    sigc::signal<void, GdkEventButton*> sig_button_press_event;

  protected:
    bool on_button_press_event (GdkEventButton* event);

  public:
    GtkListViewHelper (Glib::RefPtr<Gtk::TreeModel> const& model);
    virtual ~GtkListViewHelper (void) {}
    sigc::signal<void, GdkEventButton*>& signal_button_press_myevent (void);
};

/* ---------------------------------------------------------------- */

template <class T>
class GuiPlannerCols : public Gtk::TreeModel::ColumnRecord
{
  public:
    GuiPlannerCols (void);

    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > icon;
    Gtk::TreeModelColumn<T> data;
};

typedef GuiPlannerCols<ApiElement const*> GuiPlannerElemCols;
typedef GuiPlannerCols<ApiSkill const*> GuiPlannerSkillCols;
typedef GuiPlannerCols<ApiCert const*> GuiPlannerCertCols;

/* ---------------------------------------------------------------- */

inline
GtkListViewHelper::GtkListViewHelper (Glib::RefPtr<Gtk::TreeModel> const& model)
  : Gtk::TreeView(model)
{
}

inline sigc::signal<void, GdkEventButton*>&
GtkListViewHelper::signal_button_press_myevent (void)
{
  return this->sig_button_press_event;
}

template <class T>
inline
GuiPlannerCols<T>::GuiPlannerCols (void)
{
  this->add(name);
  this->add(icon);
  this->add(data);
}

#endif /* GTK_PLANNER_BASE_HEADER */
