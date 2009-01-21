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

#include <string>
#include <map>
#include <gtkmm/window.h>

#include "gtkinfodisplay.h"
#include "exception.h"
#include "asynchttp.h"
#include "xml.h"

/* This is the amount of milli seconds between version checks. */
#define VERSION_CHECK_INTERVAL 7200000 /* Every 2 hours. */

/* The location of versions.xml, the version information file. */
#define VERSION_CHECK_HOST "gtkevemon.battleclinic.com"
#define VERSION_CHECK_PATH "/updates/versions.xml"

/* Version information for applications. */
struct VersionInfoApp
{
  std::string name;
  std::string version;
  std::string message;
  std::string date;
};

/* Version information for data files. */
struct VersionInfoFile
{
  std::string name;
  std::string version;
  std::string message;
  std::string date;
  std::string url;
  std::string md5;
  std::string size;
};

typedef std::map<std::string, VersionInfoApp> VersionInfoAppList;
typedef std::map<std::string, VersionInfoFile> VersionInfoFileList;

/* This class keeps the requested version information. */
class VersionInformation : public XmlBase
{
  private:
    void parse_applications (xmlNodePtr node);
    void parse_datafiles (xmlNodePtr node);

  public:
    VersionInfoAppList applications;
    VersionInfoFileList datafiles;

  public:
    VersionInformation (void);
    VersionInformation (std::string const& xml_data);
    void debug_dump (void);
};

/* ---------------------------------------------------------------- */

/* Base class for requesting version information from the internet. */
class VersionCheckerBase
{
  private:
    sigc::connection request_conn;

  private:
    void handle_result (AsyncHttpData result);

  protected:
    VersionCheckerBase (void);
    virtual ~VersionCheckerBase (void);

    virtual void request_versions (void);
    virtual void handle_version_info (VersionInformation& vi) = 0;
    virtual void handle_version_error (Exception& e) = 0;
};

/* ---------------------------------------------------------------- */

/* If this class is created, it requests the version regularly.
 * If a new version is spotted, the update GUI is raised and the
 * user can optionally update the local data files.
 * When the timer expires, the version is requested for the first time,
 * not directly after creating this class. Use request_versions() for a
 * first check.
 */
class VersionChecker : public VersionCheckerBase
{
  private:
    sigc::connection timeout_conn;
    Gtk::Window* parent_window;
    GtkInfoDisplay* info_display;

  private:
    void check_gtkevemon_version (VersionInformation& vi);
    void check_datafile_versions (VersionInformation& vi);
    void handle_version_info (VersionInformation& vi);
    void handle_version_error (Exception& e);
    void show_notification (std::string const& title, std::string const& msg);

  protected:
    bool on_check_timeout (void);

  public:
    VersionChecker (void);
    ~VersionChecker (void);

    void request_versions (void);
    void set_info_display (GtkInfoDisplay* disp);
    void set_parent_window (Gtk::Window* disp);

  public:
    /* Checks if local versions are available and if the files exist.
     * It will raise the update GUI and force the user to download
     * the data files if there are no local files available.
     * This does *NOT* request version information from the internet.
     */
    static void check_data_files (void);
};


/* ---------------------------------------------------------------- */

inline
VersionInformation::VersionInformation (void)
{
}

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
