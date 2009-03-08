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

#ifndef GTK_CHAR_PAGE_HEADER
#define GTK_CHAR_PAGE_HEADER

#include <string>
#include <gdkmm/pixbuf.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/treeview.h>
#include <gtkmm/image.h>
#include <gtkmm/frame.h>
#include <gtkmm/treestore.h>
#include <gtkmm/statusicon.h>
#include <gtkmm/tooltip.h>

#include "eveapi.h"
#include "apiintraining.h"
#include "apicharsheet.h"
#include "gtkportrait.h"
#include "gtkinfodisplay.h"

/* Update remaining time every this milli seconds. */
#define CHARPAGE_REMAINING_UPDATE 1000
/* Update the live SP label every this milli seconds. */
#define CHARPAGE_LIVE_SP_LABEL_UPDATE 1000
/* Update the live SP image every this milli seconds. */
#define CHARPAGE_LIVE_SP_IMAGE_UPDATE 60000
/* Check for expired sheets every this milli seconds. */
#define CHARPAGE_CHECK_EXPIRED_SHEETS 600000
/* Update the cached duration this milli seconds. */
#define CHARPAGE_UPDATE_CACHED_DURATION 25000

class GtkCharSkillsCols : public Gtk::TreeModel::ColumnRecord
{
  public:
    GtkCharSkillsCols (void);

    Gtk::TreeModelColumn<int> id;
    Gtk::TreeModelColumn<ApiCharSheetSkill*> skill;
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<Glib::ustring> points;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > level;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > icon;
};

/* ---------------------------------------------------------------- */

struct SkillInTrainingInfo
{
  /* Updated on refresh. */
  ApiCharSheetSkill* char_skill;
  double sp_per_hour;
  unsigned int total_sp;
  unsigned int skills_at[6];
  unsigned int skill_group_sp;
  Gtk::TreeIter tree_skill_iter;
  Gtk::TreeIter tree_group_iter;

  SkillInTrainingInfo (void);
};

/* ---------------------------------------------------------------- */

class GtkCharPage : public Gtk::VBox
{
  private:
    /* Auth information. */
    EveApiAuth character;
    /* HTTP requester objects. */
    EveApiFetcher sheet_fetcher;
    EveApiFetcher training_fetcher;
    /* API data cache. */
    ApiCharSheetPtr sheet;
    ApiInTrainingPtr training;
    /* Cached worked up information. */
    SkillInTrainingInfo skill_info;

    /* GUI stuff. */
    Gtk::Window* parent_window;
    Gtk::Label char_name_label;
    Gtk::Label char_info_label;
    Gtk::Label corp_label;
    Gtk::Label balance_label;
    Gtk::Label skill_points_label;
    Gtk::Label known_skills_label;
    Gtk::Label attr_cha_label;
    Gtk::Label attr_int_label;
    Gtk::Label attr_per_label;
    Gtk::Label attr_mem_label;
    Gtk::Label attr_wil_label;
    Gtk::Label training_label;
    Gtk::Label remaining_label;
    Gtk::Label finish_eve_label;
    Gtk::Label finish_local_label;
    Gtk::Label spph_label;
    Gtk::Label live_sp_label;
    Gtk::Label charsheet_info_label;
    Gtk::Label trainsheet_info_label;
    Gtk::Button refresh_but;
    Gtk::Button info_but;
    GtkPortrait char_image;

    GtkCharSkillsCols skill_cols;
    Glib::RefPtr<Gtk::TreeStore> skill_store;
    Gtk::TreeView skill_view;
    Glib::RefPtr<Gtk::StatusIcon> tray_notify;

    GtkInfoDisplay info_display;

    /* Signals. */
    sigc::signal<void, EveApiAuth> sig_close_request;
    sigc::signal<void, EveApiAuth> sig_sheet_updated;

    /* Helpers, signal handlers, etc. */
    void update_charsheet_details (void);
    void update_training_details (void);
    void update_skill_list (void);

    /* Request and process EVE API documents. */
    void request_documents (void);
    bool check_expired_sheets (void);
    void on_charsheet_available (EveApiData data);
    void on_intraining_available (EveApiData data);

    /* Error dialogs. */
    void on_skilltree_error (std::string const& e);
    void on_charsheet_error (std::string const& e, bool cached = false);
    void on_intraining_error (std::string const& e, bool cached = false);
    void popup_error_dialog (std::string const& title,
        std::string const& heading, std::string const& message);

    /* Misc GUI stuff. */
    bool update_remaining (void);
    bool update_cached_duration (void);
    void api_info_changed (void);
    void remove_tray_notify (void);
    void create_tray_notify (void);
    void exec_notification_handler (void);
    void on_skill_completed (void);
    void on_close_clicked (void);
    void on_info_clicked (void);
    bool on_query_skillview_tooltip (int x, int y, bool key,
        Glib::RefPtr<Gtk::Tooltip> const& tooltip);
    void on_skill_activated (Gtk::TreeModel::Path const& path,
        Gtk::TreeViewColumn* col);

    bool on_live_sp_value_update (void);
    bool on_live_sp_image_update (void);
    bool calc_live_values (double& level_sp, double& total_sp, double& frac);

    /* Functions to toggle between a paitence information and
     * the real skill list (active during HTTP requests). */
    void set_skilllist_info (Gtk::Widget& widget);
    void set_skill_list (void);

  public:
    GtkCharPage (void);

    /* Sets the character. */
    void set_character (EveApiAuth const& character);
    EveApiAuth const& get_character (void);

    /* Worked up information. */
    std::string get_char_name (void);
    std::string get_tooltip_text (bool detailed);
    std::string get_skill_in_training (void);
    double get_spph_in_training (void);
    std::string get_skill_remaining (bool slim = false);
    void set_parent_window (Gtk::Window* parent);
    void open_skill_planner (void);
    void open_source_viewer (void);
    void open_info_exporter (void);

    /* Pass close requests to the outside. */
    sigc::signal<void, EveApiAuth>& signal_close_request (void);
    sigc::signal<void, EveApiAuth>& signal_sheet_updated (void);
};

/* ---------------------------------------------------------------- */

inline
GtkCharSkillsCols::GtkCharSkillsCols (void)
{
  this->add(this->id);
  this->add(this->skill);
  this->add(this->name);
  this->add(this->points);
  this->add(this->level);
  this->add(this->icon);
}

inline
SkillInTrainingInfo::SkillInTrainingInfo (void)
{
  this->char_skill = 0;
  this->sp_per_hour = 0;
  this->total_sp = 0;
  for (unsigned int i = 0; i < 6; ++i)
    this->skills_at[i] = 0;
}

inline void
GtkCharPage::set_parent_window (Gtk::Window* parent)
{
  this->parent_window = parent;
}

inline EveApiAuth const&
GtkCharPage::get_character (void)
{
  return this->character;
}

inline sigc::signal<void, EveApiAuth>&
GtkCharPage::signal_close_request (void)
{
  return this->sig_close_request;
}

inline sigc::signal<void, EveApiAuth>&
GtkCharPage::signal_sheet_updated (void)
{
  return this->sig_sheet_updated;
}

#endif /* GTK_CHAR_PAGE_HEADER */
