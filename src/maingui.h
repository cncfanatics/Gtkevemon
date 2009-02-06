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

#ifndef MAIN_GUI_HEADER
#define MAIN_GUI_HEADER

#include <vector>
#include <gtkmm/window.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/notebook.h>
#include <gtkmm/statusicon.h>

#include "gtkinfodisplay.h"
#include "versionchecker.h"
#include "gtkserver.h"
#include "eveapi.h"

/* Refresh the list of servers every this milli seconds. */
#define MAINGUI_SERVER_REFRESH 600000
/* Update the EVE time and the local time this milli seconds. */
#define MAINGUI_TIME_UPDATE 1000
/* Update the tooltip for the tray icon this milli seconds. */
#define MAINGUI_TOOLTIP_UPDATE 30000
/* Update the window title this milli seconds. */
#define MAINGUI_WINDOWTITLE_UPDATE 5000

class MainGui : public Gtk::Window
{
  private:
    ConfValuePtr conf_windowtitle;
    VersionChecker versionchecker;
    std::vector<GtkServer*> gtkserver;
    Glib::RefPtr<Gtk::ActionGroup> actions;
    Glib::RefPtr<Gtk::UIManager> uiman;
    Glib::RefPtr<Gtk::StatusIcon> tray;
    Gtk::Notebook notebook;
    Gtk::Label evetime_label;
    Gtk::Label localtime_label;
    GtkInfoDisplay info_display;
    bool iconified;

    void init_from_config (void);
    bool update_servers (void);
    bool refresh_servers (void);
    bool update_time (void);
    bool update_tooltip (void);
    bool update_windowtitle (void);
    void update_char_page (EveApiAuth const& auth);
    void setup_profile (void);
    void configuration (void);
    void about_dialog (void);
    void version_checker (void);
    void check_if_no_pages (void);
    void launch_eve (void);
    void close (void);
    bool on_window_state_event (GdkEventWindowState* event);
    void on_tray_icon_clicked (void);
    bool on_delete_event (GdkEventAny* event);
    void on_pages_changed (Gtk::Widget* widget, guint pnum);
    void on_pages_switched (GtkNotebookPage* page, guint pnum);
    void tray_popup_menu (guint button, guint32 activate_time);
    void update_tray_settings (void);
    void create_tray_icon (void);
    void destroy_tray_icon (void);
    void create_skillplan (void);
    void view_xml_source (void);
    void export_char_info (void);
    bool internal_add_character (EveApiAuth const& auth);
    bool internal_remove_character (EveApiAuth const& auth);

  public:
    MainGui (void);
    ~MainGui (void);
    void add_character (EveApiAuth const& auth);
    void remove_character (EveApiAuth const& auth);
};

#endif /* MAIN_GUI_HEADER */
