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

#ifndef VERSION_CHECKER_HEADER
#define VERSION_CHECKER_HEADER

#include <gtkmm/window.h>

#include "gtkinfodisplay.h"
#include "asynchttp.h"

/* This is the amount of milli seconds between version checks. */
#define VERSION_CHECK_INTERVAL 7200000 /* Every 2 hours. */

class VersionChecker
{
  private:
    Gtk::Window* parent_window;
    GtkInfoDisplay* info_display;

    sigc::connection request_svn_conn;
    void handle_svn_result (AsyncHttpData result);

    sigc::connection request_data_conn;
    void handle_data_result (AsyncHttpData result);

  public:
    VersionChecker (void);
    ~VersionChecker (void);

    void set_info_display (GtkInfoDisplay* disp);
    void set_parent_window (Gtk::Window* disp);
    bool request_versions (void);
};

/* ---------------------------------------------------------------- */

inline void
VersionChecker::set_info_display (GtkInfoDisplay* disp)
{
  this->info_display = disp;
}

inline void
VersionChecker::set_parent_window (Gtk::Window* parent)
{
  this->parent_window = parent;
}

#endif /* VERSION_CHECKER_HEADER */
