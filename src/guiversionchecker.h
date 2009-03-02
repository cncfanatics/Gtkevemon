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

#ifndef GUI_VERSION_CHECKER_HEADER
#define GUI_VERSION_CHECKER_HEADER

#include <vector>
#include <gtkmm/button.h>
#include <gtkmm/box.h>
#include <gtkmm/window.h>

#include "asynchttp.h"
#include "winbase.h"
#include "gtkdownloader.h"
#include "versionchecker.h"

class GuiVersionChecker : public VersionCheckerBase, public WinBase
{
  private:
    bool startup_mode;
    bool is_updated;

    VersionInformation version_info;
    std::vector<VersionInfoFile> update_list;

    GtkDownloader downloader;
    Gtk::Button* close_but;
    Gtk::Button* update_but;
    Gtk::VBox files_box;

  protected:
    void rebuild_files_box (void);
    void on_update_clicked (void);
    void on_update_done (void);
    void on_close_clicked (void);
    void on_config_clicked (void);
    void on_download_done (DownloadItem dl, AsyncHttpData data);

  public:
    GuiVersionChecker (bool startup_mode = false);
    ~GuiVersionChecker (void);

    void handle_version_info (VersionInformation& vi);
    void handle_version_error (Exception& e);
    void request_versions (void);
};

#endif /* GUI_VERSION_CHECKER_HEADER */
