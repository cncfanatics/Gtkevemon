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

#include <gtkmm/notebook.h>

#include "winbase.h"
#include "gtkplannerbase.h"
#include "gtkitemdetails.h"
#include "gtkitembrowser.h"
#include "gtktrainingplan.h"
#include "apiskilltree.h"
#include "apicharsheet.h"
#include "apiintraining.h"

class GuiSkillPlanner : public WinBase
{
  private:
    GtkSkillBrowser skill_browser;
    GtkCertBrowser cert_browser;
    GtkItemDetails details_gui;
    GtkTrainingPlan plan_gui;

    /* Character stuff. */
    ApiCharSheetPtr charsheet;
    ApiInTrainingPtr training;

    /* Misc. */
    Gtk::Notebook details_nb;
    Gtk::Notebook browser_nb;

    void on_element_selected (ApiElement const* elem);
    void on_element_activated (ApiElement const* elem);
    void on_planning_requested (ApiSkill const* skill, int level);

    bool on_gtkmain_quit (void);

  public:
    GuiSkillPlanner (void);
    ~GuiSkillPlanner (void);

    void set_character (ApiCharSheetPtr sheet);
    void set_training (ApiInTrainingPtr training);
};

#endif /* GUI_SKILL_PLANNER_HEADER */
