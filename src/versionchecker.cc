#include <sys/stat.h>
#include <iostream>
#include <glibmm.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/main.h>

#include "config.h"
#include "defines.h"
#include "guiversionchecker.h"
#include "versionchecker.h"

VersionInformation::VersionInformation (std::string const& xml_data)
{
  std::cout << "Parsing XML: versions.xml ..." << std::endl;

  XmlDocumentPtr xml = XmlDocument::create(xml_data);
  xmlNodePtr root = xml->get_root_element();

  if (root->type != XML_ELEMENT_NODE
      || xmlStrcmp(root->name, (xmlChar const*)"VersionInformation"))
    throw Exception("Invalid root tag. Expecting <VersionInformation>");

  for (xmlNodePtr node = root->children; node != 0; node = node->next)
  {
    if (node->type != XML_ELEMENT_NODE)
      continue;

    if (!xmlStrcmp(node->name, (xmlChar const*)"Applications"))
      this->parse_applications(node->children);
    else if (!xmlStrcmp(node->name, (xmlChar const*)"DataFiles"))
      this->parse_datafiles(node->children);
  }
}

/* ---------------------------------------------------------------- */

void
VersionInformation::parse_applications (xmlNodePtr node)
{
  /* Parse applications. */
  for (; node != 0; node = node->next)
  {
    if (node->type != XML_ELEMENT_NODE)
      continue;

    if (!xmlStrcmp(node->name, (xmlChar const*)"Application"))
    {
      VersionInfoApp via;
      via.name = this->get_property(node, "name");

      for (xmlNodePtr app = node->children; app != 0; app = app->next)
      {
        this->set_string_if_node_text(app, "Version", via.version);
        this->set_string_if_node_text(app, "Message", via.message);
        this->set_string_if_node_text(app, "Date", via.date);
      }

      this->applications.insert(std::make_pair(via.name, via));
    }
  }
}

/* ---------------------------------------------------------------- */

void
VersionInformation::parse_datafiles (xmlNodePtr node)
{
  /* Parse data files. */
  for (; node != 0; node = node->next)
  {
    if (node->type != XML_ELEMENT_NODE)
      continue;

    if (!xmlStrcmp(node->name, (xmlChar const*)"DataFile"))
    {
      VersionInfoFile vif;
      vif.name = this->get_property(node, "name");

      for (xmlNodePtr file = node->children; file != 0; file = file->next)
      {
        this->set_string_if_node_text(file, "Version", vif.version);
        this->set_string_if_node_text(file, "Message", vif.message);
        this->set_string_if_node_text(file, "Date", vif.date);
        this->set_string_if_node_text(file, "URL", vif.url);
        this->set_string_if_node_text(file, "MD5", vif.md5);
        this->set_string_if_node_text(file, "Size", vif.size);
      }

      this->datafiles.insert(std::make_pair(vif.name, vif));
    }
  }
}

/* ---------------------------------------------------------------- */

void
VersionInformation::debug_dump (void)
{
  std::cout << "Version information for applications available:" << std::endl;
  for (VersionInfoAppList::iterator iter = this->applications.begin();
      iter != this->applications.end(); iter++)
  {
    std::cout << "Application: " << iter->first << std::endl;
    std::cout << "  Version: " << iter->second.version << std::endl;
    std::cout << "  Message: " << iter->second.message << std::endl;
    std::cout << "  Date: " << iter->second.date << std::endl;
  }

  std::cout << "Version information for data files available:" << std::endl;
  for (VersionInfoFileList::iterator iter = this->datafiles.begin();
      iter != this->datafiles.end(); iter++)
  {
    std::cout << "Data file: " << iter->first << std::endl;
    std::cout << "  Version: " << iter->second.version << std::endl;
    std::cout << "  Message: " << iter->second.message << std::endl;
    std::cout << "  Date: " << iter->second.date << std::endl;
  }
}

/* ================================================================ */

VersionCheckerBase::VersionCheckerBase (void)
{
}

/* ---------------------------------------------------------------- */

VersionCheckerBase::~VersionCheckerBase (void)
{
  this->request_conn.disconnect();
}

/* ---------------------------------------------------------------- */

void
VersionCheckerBase::request_versions (void)
{
  std::cout << "Request XML: versions.xml ..." << std::endl;

  AsyncHttp* http = AsyncHttp::create();
  http->set_host(VERSION_CHECK_HOST);
  http->set_path(VERSION_CHECK_PATH);
  Config::setup_http(http);
  this->request_conn = http->signal_done().connect(sigc::mem_fun
      (*this, &VersionCheckerBase::handle_result));
  http->async_request();
}

/* ---------------------------------------------------------------- */

void
VersionCheckerBase::handle_result (AsyncHttpData result)
{
  if (result.data.get() == 0 || result.data->http_code != 200)
  {
    this->handle_version_error(result.exception);
    return;
  }

  #if 0
  std::cout << "Received response: " << result.data->data << std::endl;
  for (unsigned int i = 0; i < result.data->headers.size(); ++i)
    std::cout << "  headers: " << result.data->headers[i] << std::endl;
  #endif

  try
  {
    /* Parse version information. */
    VersionInformation vi(&result.data->data[0]);

    /* Pass version information to subclasses. */
    this->handle_version_info(vi);
  }
  catch (Exception& e)
  {
    std::cout << "Error parsing version information: " << e << std::endl;
  }
}

/* ================================================================ */

VersionChecker::VersionChecker (void)
{
  this->info_display = 0;
  this->parent_window = 0;
  this->timeout_conn = Glib::signal_timeout().connect(sigc::mem_fun(*this,
      &VersionChecker::on_check_timeout), VERSION_CHECK_INTERVAL);
}

/* ---------------------------------------------------------------- */

VersionChecker::~VersionChecker (void)
{
  this->timeout_conn.disconnect();
}

/* ---------------------------------------------------------------- */

bool
VersionChecker::on_check_timeout (void)
{
  this->request_versions();
  return true;
}

/* ---------------------------------------------------------------- */

void
VersionChecker::request_versions (void)
{
  /* Request SVN version. */
  ConfValuePtr check = Config::conf.get_value("versionchecker.enabled");
  if (check->get_bool())
  {
    this->VersionCheckerBase::request_versions();
  }
}

/* ---------------------------------------------------------------- */

void
VersionChecker::handle_version_info (VersionInformation& vi)
{
  this->check_gtkevemon_version(vi);
  this->check_datafile_versions(vi);
}

/* ---------------------------------------------------------------- */

void
VersionChecker::handle_version_error (Exception& e)
{
  std::cout << "Unable to perform version check: " << e << std::endl;
}

/* ---------------------------------------------------------------- */

void
VersionChecker::check_gtkevemon_version (VersionInformation& vi)
{
  #define INVALID_RESPONSE "Version checker received an invalid response!"

  /* Check if we have version information for GtkEveMon. */
  VersionInfoAppList::iterator iter = vi.applications.find("GtkEveMon");
  if (iter == vi.applications.end())
  {
    this->info_display->append(INFO_WARNING, INVALID_RESPONSE,
        "Cannot find SVN version information in the response.");
    return;
  }

  Glib::ustring cur_version(GTKEVEMON_VERSION_STR);
  Glib::ustring svn_version(iter->second.version);

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

  ConfSectionPtr seen = Config::conf.get_section("versionchecker.last_seen");
  conf_values_t::iterator siter = seen->find_value("GtkEveMon");

  if (siter != seen->values_end() && svn_version == siter->second->get_string())
    return;

  /* Mark current version as seen. Inform user about the new version. */
  ConfValuePtr seen_value = ConfValue::create(svn_version);
  seen->add("GtkEveMon", seen_value);
  Config::save_to_file();

  Glib::ustring title_text = "The current SVN version is " + svn_version;
  Glib::ustring detailed_text =
      "A new version of GtkEveMon is available "
      "in SVN! Please consider updating to the latest version.\n\n"
      "Your local version is: " + cur_version + "\n"
      "The latest version is: " + svn_version + "\n\n"
      "Update message: " + iter->second.message + "\n\n"
      "Take a look at the forums for further information:\n"
      "http://www.battleclinic.com/forum/index.php#c43";
  this->show_notification(title_text, detailed_text);
}

/* ---------------------------------------------------------------- */

void
VersionChecker::check_datafile_versions (VersionInformation& vi)
{
  std::vector<std::string> announce;

  /* Locate information about the local files. */
  ConfSectionPtr files = Config::conf.get_section("versionchecker.datafiles");
  for (conf_values_t::iterator iter = files->values_begin();
      iter != files->values_end(); iter++)
  {
    std::string name = iter->first;
    std::string version = iter->second->get_string();

    /* See if the local file is available in the version information. */
    VersionInfoFileList::iterator info = vi.datafiles.find(name);
    if (info == vi.datafiles.end())
      continue;

    /* Version information is available. Check if local file is outdated. */
    if (info->second.version == version)
      continue;

    /* Local file is outdated. Check if this version was seen before. */
    ConfSectionPtr seen = Config::conf.get_section("versionchecker.last_seen");
    conf_values_t::iterator siter = seen->find_value(name);
    if (siter != seen->values_end()
        && siter->second->get_string() == info->second.version)
      continue;

    /* Recent version is to be announced. */
    announce.push_back(name);

    /* Mark version as seen. */
    ConfValuePtr seen_value = ConfValue::create(info->second.version);
    seen->add(name, seen_value);
  }

  if (announce.empty())
    return;

  /* Save the config file because there are new "last seen" values. */
  Config::save_to_file();

  /* A local file is to be announced. Raise GUI or display a notification. */
  if (Config::conf.get_value("versionchecker.raise_updater")->get_bool())
  {
    GuiVersionChecker* guivc = new GuiVersionChecker();
    if (this->parent_window)
      guivc->set_transient_for(*this->parent_window);
    guivc->handle_version_info(vi);
  }
  else
  {
    std::string title_text =
        "Recent versions for some data files are available!";
    std::string detailed_text =
        "You can update data files from the \"EveMon\" menu. "
        "There are recent versions for the following data files:\n";
    for (unsigned int i = 0; i < announce.size(); ++i)
      detailed_text += "\n" + announce[i];
    this->show_notification(title_text, detailed_text);
  }
}

/* ---------------------------------------------------------------- */

void
VersionChecker::show_notification (std::string const& title,
    std::string const& msg)
{
  if (this->info_display != 0)
  {
    this->info_display->append(INFO_NOTIFICATION, title, msg);
  }
  else
  {
    Gtk::MessageDialog md(title, false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
    md.set_secondary_text(msg);
    md.set_title("Version check - GtkEveMon");
    if (this->parent_window != 0)
      md.set_transient_for(*this->parent_window);
    md.run();
  }
}

/* ---------------------------------------------------------------- */

void
VersionChecker::check_data_files (void)
{
  bool force_update = false;

  ConfSectionPtr files = Config::conf.get_section("versionchecker.datafiles");
  for (conf_values_t::iterator iter = files->values_begin();
      iter != files->values_end(); iter++)
  {
    std::string name = iter->first;
    std::string version = iter->second->get_string();
    std::string filename = Config::get_conf_dir() + "/" + name;
    if (version.empty())
    {
      force_update = true;
    }
    else
    {
      struct stat statbuf;
      if (::stat(filename.c_str(), &statbuf) < 0)
      {
        iter->second->get_string().clear();
        force_update = true;
      }
    }
  }

  if (force_update)
  {
    new GuiVersionChecker(true);
    Gtk::Main::run();
  }
}
