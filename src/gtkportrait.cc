#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <iostream>

#include "http.h"
#include "imagestore.h"
#include "config.h"
#include "gtkportrait.h"

sigc::signal<void> GtkPortrait::sig_update_portraits;

/* ---------------------------------------------------------------- */

void
GtkPortrait::emit_update_signal (void)
{
  GtkPortrait::sig_update_portraits.emit();
}

/* ---------------------------------------------------------------- */

GtkPortrait::GtkPortrait (void)
{
  this->update_con = GtkPortrait::sig_update_portraits.connect
      (sigc::mem_fun(*this, &GtkPortrait::update));
}

/* ---------------------------------------------------------------- */

GtkPortrait::GtkPortrait (std::string const& charid)
{
  this->set(charid);
  this->update_con = GtkPortrait::sig_update_portraits.connect
      (sigc::mem_fun(*this, &GtkPortrait::update));
}

/* ---------------------------------------------------------------- */

GtkPortrait::~GtkPortrait (void)
{
  this->update_con.disconnect();
}

/* ---------------------------------------------------------------- */

void
GtkPortrait::set (std::string const& charid)
{
  this->char_id = charid;

  bool success = false;

  /* Try to get the portrait from our own cache. */
  if (!success)
    success = this->fetch_from_gtkevemon_cache();

  /* Then try to get it from the EVE cache. */
  if (!success)
    success = this->fetch_from_eve_cache();

  /* If there is no portrait, get it online. */
  if (!success)
    success = this->fetch_from_eve_online();

  /* Nothing worked. Fall back to default image. */
  if (!success)
  {
    std::cout << "Could not fetch protrait! Falling back." << std::endl;

    Glib::RefPtr<Gdk::Pixbuf> pixbuf = ImageStore::eveportrait;
    Glib::RefPtr<Gdk::Pixbuf> scaled = pixbuf->scale_simple
        (PORTRAIT_SIZE, PORTRAIT_SIZE, Gdk::INTERP_BILINEAR);
    this->Gtk::Image::set(scaled);
  }
}

/* ---------------------------------------------------------------- */

void
GtkPortrait::update (void)
{
  this->set(this->char_id);
}

/* ---------------------------------------------------------------- */

bool
GtkPortrait::fetch_from_gtkevemon_cache (void)
{
  bool success = false;

  try
  {
    std::string portraitdir = Config::get_conf_dir() + "/portraits";
    std::stringstream filename;
    filename << portraitdir << "/" << this->char_id
        << "_" << PORTRAIT_SIZE << ".png";

    Glib::RefPtr<Gdk::Pixbuf> portrait = Gdk::Pixbuf::create_from_file
        (filename.str());
    this->Gtk::Image::set(portrait);
    success = true;
  }
  catch (...)
  {
  }

  //if (success)
  //  std::cout << "Using protrait from GtkEveMon cache!" << std::endl;

  return success;
}

/* ---------------------------------------------------------------- */

bool
GtkPortrait::fetch_from_eve_cache (void)
{
  bool success = false;

  /* Search for these image sizes. */
  std::vector<std::string> sizes;
  sizes.push_back("_256.png");
  sizes.push_back("_128.png");

  std::string basepath = **Config::conf.get_value("settings.eve_cache");
  basepath += "/Pictures/Portraits/";

  for (unsigned int i = 0; i < sizes.size(); ++i)
  {
    std::string filename = this->char_id + sizes[i];
    try
    {
      std::string path = basepath + filename;
      Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_file(path);
      Glib::RefPtr<Gdk::Pixbuf> scaled = pixbuf->scale_simple
         (PORTRAIT_SIZE, PORTRAIT_SIZE, Gdk::INTERP_BILINEAR);
      this->Gtk::Image::set(scaled);
      this->cache_portrait(scaled);
      success = true;
      break;
    }
    catch (Glib::FileError& e)
    {
      /* This exception is fired when the desired file is not available. */
    }
    catch (Gdk::PixbufError& e)
    {
      /* Error loading an image file. Strange things happened. */
      std::cout << "Error creating portrait from " << filename
          << ": Code " << e.code() << std::endl;
    }
    catch (...)
    {
      /* Some other unknown error occurred. */
      std::cout << "Unknown error creating portrait from "
          << filename << std::endl;
    }
  }

  //if (success)
  //  std::cout << "Fetched portrait from the EVE cache!" << std::endl;

  return success;
}

/* ---------------------------------------------------------------- */
/* Multiiple problems:
 *   * Dir creation for the temp JPG
 */

bool
GtkPortrait::fetch_from_eve_online (void)
{
  bool success = false;

  std::cout << "Requesting portrait: " << this->char_id
      << " ..." << std::endl;

  /* Request the image over HTTP. */
  HttpDataPtr portrait;
  try
  {
    Http fetcher("img.eve.is", "/serv.asp?s=256&c=" + this->char_id);
    portrait = fetcher.request();
  }
  catch (...)
  {
    std::cout << "Error fetching portrait from EVE Online!" << std::endl;
    return false;
  }

  /* Generate filenames to store the fetched JPG and the destination PNG. */
  std::string confdir = Config::get_conf_dir();
  std::string portraitdir = confdir + "/portraits";

  std::stringstream jpg_name;
  jpg_name << confdir << "/" << this->char_id << "_256.jpg";

  std::stringstream png_name;
  png_name << portraitdir << "/" << this->char_id
      << "_" << PORTRAIT_SIZE << ".png";

  try
  {
    std::ofstream out(jpg_name.str().c_str());
    out.write(portrait->data, portrait->size);
    out.close();

    Gdk::Pixbuf::create_from_file(jpg_name.str())
        ->scale_simple(PORTRAIT_SIZE, PORTRAIT_SIZE, Gdk::INTERP_BILINEAR)
        ->save(png_name.str(), "png");

    ::unlink(jpg_name.str().c_str());
  }
  catch (...)
  {
    std::cout << "Error saving portrait from EVE Online" << std::endl;
    return false;
  }

  success = this->fetch_from_gtkevemon_cache();

  //if (success)
  //  std::cout << "Fetched portrait from EVE Online!" << std::endl;

  return success;
}

/* ---------------------------------------------------------------- */

void
GtkPortrait::cache_portrait (Glib::RefPtr<Gdk::Pixbuf> portrait)
{
  std::string portraitdir = Config::get_conf_dir() + "/portraits";

  /* Create portrait directory if it does not exist. */
  int dir_exists = ::access(portraitdir.c_str(), F_OK);
  if (dir_exists < 0)
  {
    /* Ignore errors. We'll get then on creation. */
    ::mkdir(portraitdir.c_str(), S_IRWXU);
  }

  try
  {
    std::stringstream filename;
    filename << portraitdir << "/" << this->char_id
        << "_" << PORTRAIT_SIZE << ".png";
    portrait->save(filename.str(), "png");
  }
  catch (...)
  {
    std::cout << "Error caching portrait for " << this->char_id << std::endl;
  }
}
