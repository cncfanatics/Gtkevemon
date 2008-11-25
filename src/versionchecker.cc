#include <iostream>
#include <glibmm.h>
#include <gtkmm/messagedialog.h>

#include "config.h"
#include "defines.h"
#include "versionchecker.h"

VersionChecker::VersionChecker (void)
{
  this->info_display = 0;
  this->parent_window = 0;
  Glib::signal_timeout().connect(sigc::mem_fun
      (*this, &VersionChecker::request_versions), VERSION_CHECK_INTERVAL);
}

/* ---------------------------------------------------------------- */

VersionChecker::~VersionChecker (void)
{
  this->request_svn_conn.disconnect();
  this->request_data_conn.disconnect();
}

/* ---------------------------------------------------------------- */

bool
VersionChecker::request_versions (void)
{
  /* Request SVN version. */
  ConfValuePtr check = Config::conf.get_value("versionchecker.svn_check");
  if (check->get_bool())
  {
    AsyncHttp* http = AsyncHttp::create();
    http->set_host("gtkevemon.battleclinic.com");
    http->set_path("/svn_version.txt");
    http->set_agent("GtkEveMon");
    this->request_svn_conn = http->signal_done().connect(sigc::mem_fun
        (*this, &VersionChecker::handle_svn_result));
    http->async_request();
  }

  #if 0
  /* Request data versions. */
  check = Config::conf.get_value("versionchecker.data_check");
  if (check->get_bool())
  {
    AsyncHttp* http = AsyncHttp::create();
    http->set_host("gtkevemon.battleclinic.com");
    http->set_path("/data/versions.txt");
    http->set_agent("GtkEveMon");
    this->request_data_conn = http->signal_done().connect(sigc::mem_fun
        (*this, &VersionChecker::handle_data_result));
    http->async_request();
  }
  #endif

  return true;
}

/* ---------------------------------------------------------------- */

void
VersionChecker::handle_svn_result (AsyncHttpData result)
{
  if (result.data.get() == 0)
  {
    std::cout << "Unable to perform SVN version check: "
        << result.exception << std::endl;
    return;
  }

  Glib::ustring cur_version(GTKEVEMON_VERSION_STR);
  Glib::ustring svn_version(result.data->data);

#define INVALID_RESPONSE "Version checker received an invalid response!"

  /* Make some sanity checks. */
  if (svn_version.empty())
  {
    this->info_display->append(INFO_WARNING, INVALID_RESPONSE,
        "The version checker received a zero-length string.");
    return;
  }

  if (svn_version.size() > 64)
  {
    this->info_display->append(INFO_WARNING, INVALID_RESPONSE,
        "The version checker received a too long string.");
    return;
  }

  size_t newline_pos = svn_version.find_first_of('\n');
  if (newline_pos != std::string::npos)
    svn_version = svn_version.substr(0, newline_pos);

  /* Check the version. */
  if (svn_version == cur_version)
    return;

  ConfValuePtr last_seen = Config::conf.get_value
      ("versionchecker.svn_last_seen");

  if (svn_version == **last_seen)
    return;

  /* Mark current version as seen. Inform user about the new version. */
  last_seen->set(svn_version);
  Config::save_to_file();

  if (this->info_display != 0)
  {
    this->info_display->append(INFO_NOTIFICATION,
        "The current SVN version is " + svn_version,
        "A new version of GtkEveMon is available "
        "in SVN! Please consider updating to the latest version.\n\n"
        "Your local version is: " + cur_version + "\n"
        "The latest version is: " + svn_version + "\n\n"
        "Take a look at the forums for further information:\n"
        "http://www.battleclinic.com/forum/index.php#c43");
  }
  else
  {
    Gtk::MessageDialog md("There is an update available!",
        false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
    md.set_secondary_text("A new version of GtkEveMon is available "
        "in SVN! Please consider updating to the latest version.\n\n"
        "Your local version is: " + cur_version + "\n"
        "The latest version is: " + svn_version + "\n\n"
        "Take a look at the forums for further information:\n"
        "http://www.battleclinic.com/forum/index.php#c43");
    md.set_title("Version check - GtkEveMon");
    if (this->parent_window != 0)
      md.set_transient_for(*this->parent_window);
    md.run();
  }
}

/* ---------------------------------------------------------------- */

void
VersionChecker::handle_data_result (AsyncHttpData result)
{
  std::cout << "Received data versions:" << std::endl;
  if (result.data.get() == 0)
  {
    std::cout << "Error requesting data: " << result.exception << std::endl;
  }
  else
  {
    std::cout << "Received response: " << result.data->data << std::endl;
    for (unsigned int i = 0; i < result.data->headers.size(); ++i)
      std::cout << "  headers: " << result.data->headers[i] << std::endl;
  }
}
