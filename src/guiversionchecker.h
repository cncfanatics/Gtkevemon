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
    void on_download_done (DownloadItem dl, AsyncHttpData data);

  public:
    GuiVersionChecker (bool startup_mode = false);
    ~GuiVersionChecker (void);

    void handle_version_info (VersionInformation& vi);
    bool request_versions (void);
};

/* ---------------------------------------------------------------- */

inline bool
GuiVersionChecker::request_versions (void)
{
  return this->VersionCheckerBase::request_versions();
}

#endif /* GUI_VERSION_CHECKER_HEADER */
