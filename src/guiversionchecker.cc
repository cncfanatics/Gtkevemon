#include <unistd.h>
#include <fstream>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <gtkmm/box.h>
#include <gtkmm/main.h>
#include <gtkmm/table.h>
#include <gtkmm/frame.h>
#include <gtkmm/stock.h>
#include <gtkmm/separator.h>

#include "config.h"
#include "helpers.h"
#include "imagestore.h"
#include "gtkdefines.h"
#include "argumentsettings.h"
#include "guiconfiguration.h"
#include "guiversionchecker.h"

GuiVersionChecker::GuiVersionChecker (bool startup_mode)
{
  this->startup_mode = startup_mode;
  this->is_updated = false;
  if (startup_mode)
    this->close_but = MK_BUT(Gtk::Stock::QUIT);
  else
    this->close_but = MK_BUT(Gtk::Stock::CLOSE);
  this->update_but = MK_BUT(Gtk::Stock::REFRESH);
  this->update_but->set_sensitive(false);
  this->files_box.set_spacing(5);

  Gtk::Button* config_but = MK_BUT0;
  config_but->set_image(*MK_IMG
      (Gtk::Stock::PREFERENCES, Gtk::ICON_SIZE_BUTTON));

  Gtk::Label* info_label = MK_LABEL0;
  info_label->set_line_wrap(true);
  info_label->set_alignment(Gtk::ALIGN_LEFT);
  if (startup_mode)
    info_label->set_text("Some data files are not available on your system. "
        "All of the following files are required for GtkEveMon to run. "
        "GtkEveMon is going to download these files now.");
  else
    info_label->set_text("Check the data files for recent versions and "
        "keep them up-to-date. After downloading, you have to restart "
        "the application.");

  Gtk::VBox* frame_box = MK_VBOX;
  frame_box->set_border_width(5);
  frame_box->pack_start(*info_label, false, false, 0);
  frame_box->pack_start(*MK_HSEP, false, false, 0);
  frame_box->pack_start(this->files_box, false, false, 0);
  //frame_box->pack_start(this->download_but, false, false, 0);
  frame_box->pack_end(this->downloader, false, false, 0);

  Gtk::Frame* main_frame = MK_FRAME0;
  main_frame->set_shadow_type(Gtk::SHADOW_OUT);
  main_frame->add(*frame_box);

  Gtk::HBox* button_box = MK_HBOX;
  button_box->pack_start(*this->close_but, false, false, 0);
  button_box->pack_end(*this->update_but, false, false, 0);
  if (this->startup_mode)
    button_box->pack_start(*config_but, false, false, 0);

  Gtk::VBox* main_vbox = MK_VBOX;
  main_vbox->pack_start(*main_frame, true, true, 0);
  main_vbox->pack_end(*button_box, false, false);

  Gtk::Image* logo_img = Gtk::manage(new Gtk::Image(ImageStore::aboutlogo));
  Gtk::Frame* logo_frame = MK_FRAME0;
  logo_frame->add(*logo_img);
  logo_frame->set_shadow_type(Gtk::SHADOW_IN);

  Gtk::HBox* main_hbox = MK_HBOX;
  main_hbox->set_border_width(5);
  main_hbox->pack_start(*logo_frame, false, false, 0);
  main_hbox->pack_start(*main_vbox, true, true, 0);

  this->close_but->signal_clicked().connect(sigc::mem_fun
      (*this, &GuiVersionChecker::on_close_clicked));
  this->update_but->signal_clicked().connect(sigc::mem_fun
      (*this, &GuiVersionChecker::on_update_clicked));
  config_but->signal_clicked().connect(sigc::mem_fun
      (*this, &GuiVersionChecker::on_config_clicked));
  this->downloader.signal_download_done().connect(sigc::mem_fun
      (*this, &GuiVersionChecker::on_download_done));
  this->downloader.signal_all_downloads_done().connect
      (sigc::mem_fun(*this, &GuiVersionChecker::on_update_done));

  this->set_title("Version Checker - GtkEveMon");
  this->set_default_size(525, -1);
  this->add(*main_hbox);
  this->show_all();
  this->downloader.hide();

  if (startup_mode)
    this->request_versions();
}

/* ---------------------------------------------------------------- */

GuiVersionChecker::~GuiVersionChecker (void)
{
  if (this->startup_mode)
    Gtk::Main::quit();

  if (this->startup_mode && !this->is_updated)
    ::exit(0);

  if (!this->startup_mode && this->is_updated)
    ::exit(0);
}

/* ---------------------------------------------------------------- */

void
GuiVersionChecker::handle_version_error (Exception& e)
{
  std::cout << "Not able to perform version check: " << e << std::endl;

  this->files_box.children().clear();

  Gtk::Label* error_label = MK_LABEL("There was a problem while "
      "requesting\nthe online versioning service.\nMessage: " + e);
  error_label->set_line_wrap(true);
  error_label->set_alignment(Gtk::ALIGN_LEFT);

  Gtk::HBox* error_box = MK_HBOX;
  error_box->set_border_width(5);
  error_box->pack_start(*MK_IMG(Gtk::Stock::DIALOG_ERROR,
      Gtk::ICON_SIZE_DIALOG), false, false, 0);
  error_box->pack_start(*error_label, true, true, 0);

  Gtk::Button* again_but = MK_BUT("Try again");
  again_but->set_image(*MK_IMG(Gtk::Stock::NETWORK, Gtk::ICON_SIZE_BUTTON));
  Gtk::HBox* button_box = MK_HBOX;
  button_box->pack_start(*again_but, true, false, 0);

  again_but->signal_clicked().connect(sigc::mem_fun
      (*this, &GuiVersionChecker::request_versions));

  this->files_box.pack_start(*error_box, false, false, 0);
  this->files_box.pack_start(*button_box, false, false, 0);
  this->files_box.show_all();
}

/* ---------------------------------------------------------------- */

void
GuiVersionChecker::handle_version_info (VersionInformation& vi)
{
  this->version_info = vi;
  this->rebuild_files_box();
  this->update_but->set_sensitive(!this->update_list.empty());
}

/* ---------------------------------------------------------------- */

void
GuiVersionChecker::request_versions (void)
{
  this->files_box.children().clear();
  this->files_box.pack_start(*MK_LABEL("Requesting version information..."));
  this->files_box.show_all();
  this->VersionCheckerBase::request_versions();
}

/* ---------------------------------------------------------------- */

void
GuiVersionChecker::rebuild_files_box (void)
{
  this->files_box.children().clear();
  this->update_list.clear();

  ConfSectionPtr files = Config::conf.get_section("versionchecker.datafiles");
  for (conf_values_t::iterator iter = files->values_begin();
      iter != files->values_end(); iter++)
  {
    std::string name = iter->first;
    std::string version = iter->second->get_string();

    VersionInfoFileList::iterator info
        = this->version_info.datafiles.find(name);

    std::string info_version;
    std::string info_size;
    Gtk::Image* status_image;
    if (info != this->version_info.datafiles.end())
    {
      info_version = info->second.version;
      info_size = Helpers::get_string_from_float
          (Helpers::get_float_from_string(info->second.size) / 1024.0f, 0);
      if (info_version == version)
        status_image = MK_IMG(Gtk::Stock::YES, Gtk::ICON_SIZE_BUTTON);
      else
      {
        status_image = MK_IMG(Gtk::Stock::NO, Gtk::ICON_SIZE_BUTTON);
        this->update_list.push_back(info->second);
      }
    }
    else
    {
      info_version = "n/a";
      info_size = "n/a";
      status_image = MK_IMG(Gtk::Stock::NO, Gtk::ICON_SIZE_BUTTON);
    }

    if (version.empty())
      version = "n/a";

    Gtk::Label* local_version = MK_LABEL("Local version: " + version);
    local_version->set_alignment(Gtk::ALIGN_RIGHT);
    Gtk::Label* current_version = MK_LABEL("Current version: " + info_version);
    current_version->set_alignment(Gtk::ALIGN_RIGHT);
    Gtk::Label* current_size = MK_LABEL("Size: " + info_size + " KB");
    current_size->set_alignment(Gtk::ALIGN_LEFT);

    Gtk::Label* filename_label = MK_LABEL("<b>" + name + "</b>");
    filename_label->set_use_markup(true);
    filename_label->set_alignment(Gtk::ALIGN_LEFT);

    Gtk::Table* entry = MK_TABLE(2, 3);
    entry->set_col_spacings(10);
    entry->attach(*status_image, 0, 1, 0, 2, Gtk::SHRINK | Gtk::FILL);
    entry->attach(*filename_label, 1, 2, 0, 1, Gtk::SHRINK | Gtk::FILL);
    entry->attach(*current_size, 1, 2, 1, 2, Gtk::SHRINK | Gtk::FILL);
    entry->attach(*local_version, 2, 3, 0, 1, Gtk::EXPAND | Gtk::FILL);
    entry->attach(*current_version, 2, 3, 1, 2, Gtk::EXPAND | Gtk::FILL);

    this->files_box.pack_start(*entry, false, false, 0);
    this->files_box.pack_start(*MK_HSEP, false, false, 0);
  }

  this->files_box.show_all();
}

/* ---------------------------------------------------------------- */

void
GuiVersionChecker::on_close_clicked (void)
{
  /* Backup startup mode flag because close will delete members. */
  bool startup_mode = this->startup_mode;

  this->close();

  if (startup_mode)
    ::exit(0);
}

/* ---------------------------------------------------------------- */

void
GuiVersionChecker::on_update_clicked (void)
{
  if (!is_updated)
  {
    for (unsigned int i = 0; i < this->update_list.size(); ++i)
    {
      DownloadItem dl;
      dl.name = this->update_list[i].name;
      dl.url = this->update_list[i].url;
      this->downloader.append_download(dl);
    }
    this->downloader.show();
    this->downloader.start_downloads();
  }
  else
  {
    if (this->startup_mode)
      this->close();
    else
    {
      ::execv(ArgumentSettings::argv[0], ArgumentSettings::argv);
      //Gtk::Main::quit(); /* Quit application */
    }
  }
}

/* ---------------------------------------------------------------- */

void
GuiVersionChecker::on_update_done (void)
{
  this->downloader.hide();
  this->rebuild_files_box();
  if (!this->update_list.empty())
    return;

  this->is_updated = true;
  Config::save_to_file();

  if (this->startup_mode)
  {
    this->update_but->set_image(*MK_IMG(Gtk::Stock::MEDIA_PLAY,
        Gtk::ICON_SIZE_BUTTON));
    this->update_but->set_label("Continue");
  }
  else
  {
    this->close_but->set_sensitive(false);
    this->update_but->set_image(*MK_IMG(Gtk::Stock::QUIT,
         Gtk::ICON_SIZE_BUTTON));
    this->update_but->set_label("Restart");
  }
}

/* ---------------------------------------------------------------- */

void
GuiVersionChecker::on_config_clicked (void)
{
  GuiConfiguration* dialog = new GuiConfiguration();
  dialog->set_transient_for(*this);
}

/* ---------------------------------------------------------------- */

void
GuiVersionChecker::on_download_done (DownloadItem dl, AsyncHttpData data)
{
  /* Download successful? */
  if (data.data.get() == 0 || data.data->http_code != 200)
  {
    std::cout << "Error: The download for " << dl.name
        << " failed: " << data.exception << std::endl;
    return;
  }

  /* Locate corresponding version info. */
  VersionInfoFileList::iterator iiter
      = this->version_info.datafiles.find(dl.name);
  if (iiter == this->version_info.datafiles.end())
  {
    std::cout << "Error: The download could not be associated" << std::endl;
    return;
  }

  VersionInfoFile& vif = iiter->second;

  /* Write the file. */
  std::cout << "Writing XML: " << vif.name << " ...";
  std::cout.flush();

  std::string conf_dir = Config::get_conf_dir();
  std::string dest_name = conf_dir + "/" + vif.name;

  std::ofstream dlf(dest_name.c_str(), std::ios::binary);
  if (dlf.fail())
  {
    std::cout << std::endl << "Error: " << ::strerror(errno) << std::endl;
    return;
  }

  dlf.write(&data.data->data[0], data.data->data.size() - 1);
  if (dlf.fail())
  {
    std::cout << std::endl << "Error: " << ::strerror(errno) << std::endl;
    dlf.close();
    return;
  }

  dlf.close();

  /* Store new version information in the configuration. */
  ConfSectionPtr sect = Config::conf.get_section("versionchecker.datafiles");
  sect->add(vif.name, ConfValue::create(vif.version));

  std::cout << " Version " << vif.version << "." << std::endl;

  this->rebuild_files_box();
}
