#ifndef GTK_CHAR_PAGE_HEADER
#define GTK_CHAR_PAGE_HEADER

#include <gdkmm/pixbuf.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/treeview.h>
#include <gtkmm/image.h>
#include <gtkmm/tooltips.h>
#include <gtkmm/frame.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treestore.h>
#include <gtkmm/statusicon.h>

#include "exception.h"
#include "gtkportrait.h"
#include "eveapi.h"
#include "apiintraining.h"
#include "apicharsheet.h"

/* Update remaining time every this milli seconds. */
#define CHARPAGE_REMAINING_UPDATE 1000
/* Update the live SP label every this milli seconds. */
#define CHARPAGE_LIVE_SP_LABEL_UPDATE 1000
/* Update the live SP image every this milli secons. */
#define CHARPAGE_LIVE_SP_IMAGE_UPDATE 60000

class GtkCharSkillsCols : public Gtk::TreeModel::ColumnRecord
{
  public:
    GtkCharSkillsCols (void);

    Gtk::TreeModelColumn<int> id;
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
  unsigned int skills_at_five;
  unsigned int skill_group_sp;
  Gtk::TreeIter tree_skill_iter;
  Gtk::TreeIter tree_group_iter;

  SkillInTrainingInfo (void);
};

/* ---------------------------------------------------------------- */

class GtkCharPage : public Gtk::VBox
{
  private:
    EveApiAuth character;

    ApiCharSheetPtr sheet;
    ApiInTrainingPtr training;
    SkillInTrainingInfo skill_info;

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
    Gtk::Button refresh_but;
    Gtk::Button info_but;
    GtkPortrait char_image;

    Gtk::Frame skill_frame;
    Gtk::ScrolledWindow scwin;
    Gtk::HBox patience_box;
    GtkCharSkillsCols skill_cols;
    Glib::RefPtr<Gtk::TreeStore> skill_store;
    Gtk::TreeView skill_view;
    Glib::RefPtr<Gtk::StatusIcon> tray_notify;

    sigc::signal<void, EveApiAuth> sig_close_request;

    void update_gui (void);
    void update_skill_list (void);
    bool update_remaining (void);
    void refresh_data (void);
    void api_info_changed (void);
    void remove_tray_notify (void);
    void create_tray_notify (void);
    void on_skill_completed (void);
    void on_close_clicked (void);
    bool on_query_skillview_tooltip (int x, int y, bool key,
        Glib::RefPtr<Gtk::Tooltip> const& tooltip);
    void on_skill_activated (Gtk::TreeModel::Path const& path,
        Gtk::TreeViewColumn* col);

    bool on_live_sp_value_update (void);
    bool on_live_sp_image_update (void);

    void set_patience_info (void);
    void set_skill_list (void);

    void popup_skilltree_error (Exception const& e);
    void popup_charsheet_error (Exception const& e);
    void popup_intraining_error (Exception const& e);
    void info_clicked (void);

  public:
    GtkCharPage (void);

    void set_character (EveApiAuth const& character);
    EveApiAuth const& get_character (void);
    std::string get_char_name (void);
    std::string get_tooltip_text (void);
    std::string get_skill_in_training (void);
    double get_spph_in_training (void);
    std::string get_skill_remaining (bool slim = false);
    void set_parent_window (Gtk::Window* parent);

    sigc::signal<void, EveApiAuth>& signal_close_request (void);
};

/* ---------------------------------------------------------------- */

inline
GtkCharSkillsCols::GtkCharSkillsCols (void)
{
  this->add(this->id);
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
  this->skills_at_five = 0;
}

inline void
GtkCharPage::set_parent_window (Gtk::Window* parent)
{
  this->parent_window = parent;
}

#endif /* GTK_CHAR_PAGE_HEADER */
