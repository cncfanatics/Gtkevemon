#ifndef MAIN_GUI_HEADER
#define MAIN_GUI_HEADER

#include <vector>
#include <gtkmm/window.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/notebook.h>
#include <gtkmm/statusicon.h>

#include "winbase.h"
#include "gtkserver.h"

/* Refresh the list of servers every this milli seconds. */
#define MAINGUI_SERVER_REFRESH 300000
/* Update the list of servers in the GUI this milli seconds. */
#define MAINGUI_SERVER_UPDATE 5000
/* Update the EVE time and the local time this milli seconds. */
#define MAINGUI_TIME_UPDATE 1000
/* Update the tooltip for the tray icon this milli seconds. */
#define MAINGUI_TOOLTIP_UPDATE 30000

class MainGui : public WinBase
{
  private:
    std::vector<GtkServer*> gtkserver;
    Glib::RefPtr<Gtk::ActionGroup> actions;
    Glib::RefPtr<Gtk::UIManager> uiman;
    Glib::RefPtr<Gtk::StatusIcon> tray;
    Gtk::Notebook notebook;
    Gtk::Label evetime_label;
    Gtk::Label localtime_label;

    void init_from_config (void);
    void store_to_config (void);
    bool update_servers (void);
    bool refresh_servers (void);
    bool update_time (void);
    bool update_tooltip (void);
    void setup_profile (void);
    void configuration (void);
    void about_dialog (void);
    void check_if_no_pages (void);
    void launch_eve (void);
    void close (void);
    bool on_window_state_event (GdkEventWindowState* event);
    bool on_delete_event (GdkEventAny* event);
    void on_tray_icon_clicked (void);
    void on_pages_changed (Gtk::Widget* widget, guint pnum);
    void tray_popup_menu (guint button, guint32 activate_time);
    void update_tray_settings (void);
    void create_tray_icon (void);
    void destroy_tray_icon (void);
    void create_skillplan (void);

  public:
    MainGui (void);
    ~MainGui (void);
    void add_character (EveApiAuth const& auth);
    void remove_character (EveApiAuth const& auth);
};

#endif /* MAIN_GUI_HEADER */
