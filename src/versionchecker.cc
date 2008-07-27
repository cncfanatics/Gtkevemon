#include <iostream>
#include <glibmm.h>
#include <gtkmm/messagedialog.h>

#include "config.h"
#include "defines.h"
#include "versionchecker.h"

VersionChecker::VersionChecker (void)
{
  this->parent_win = 0;
  Glib::signal_timeout().connect(sigc::mem_fun
      (*this, &VersionChecker::request_version), VERSION_CHECK_INTERVAL);
}

/* ---------------------------------------------------------------- */

VersionChecker::~VersionChecker (void)
{
  this->request_conn.disconnect();
}

/* ---------------------------------------------------------------- */

bool
VersionChecker::request_version (void)
{
  ConfValuePtr check = Config::conf.get_value("versionchecker.enabled");
  if (!check->get_bool())
    return true;

  AsyncHttp* http = AsyncHttp::create();
  http->set_host("gtkevemon.battleclinic.com");
  http->set_path("/svn_version.txt");
  http->set_agent("GtkEveMon");
  this->request_conn = http->signal_done().connect(sigc::mem_fun
      (*this, &VersionChecker::handle_result));
  http->async_request();

  return true;
}

/* ---------------------------------------------------------------- */

void
VersionChecker::handle_result (AsyncHttpData result)
{
  if (result.data.get() == 0)
  {
    std::cout << "Unable to perform version check: "
        << result.exception << std::endl;
    return;
  }

  Glib::ustring cur_version(GTKEVEMON_VERSION_STR);
  Glib::ustring svn_version(result.data->data);

  if (svn_version == cur_version)
    return;

  ConfValuePtr last_seen = Config::conf.get_value("versionchecker.last_seen");
  if (svn_version == **last_seen)
    return;

  /* Mark current version as seen. Inform user about the new version. */
  last_seen->set(svn_version);
  Config::save_to_file();

  Gtk::MessageDialog md("There is an update available!",
      false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
  md.set_secondary_text("A new version of GtkEveMon is available "
      "in SVN! Please consider updating to the latest version.\n\n"
      "Your local version is: " + cur_version + "\n"
      "The latest version is: " + svn_version + "\n\n"
      "Take a look at the forums for further information:\n"
      "http://www.battleclinic.com/forum/index.php#43");
  md.set_title("Version check - GtkEveMon");
  if (this->parent_win != 0)
    md.set_transient_for(*this->parent_win);
  md.run();
}
