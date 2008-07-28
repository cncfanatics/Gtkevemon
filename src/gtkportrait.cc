#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <iostream>

#include "http.h"
#include "imagestore.h"
#include "config.h"
#include "gtkportrait.h"

GtkPortrait::GtkPortrait (void)
{
}

/* ---------------------------------------------------------------- */

GtkPortrait::GtkPortrait (std::string const& charid)
{
  this->set(charid);
}

/* ---------------------------------------------------------------- */

GtkPortrait::~GtkPortrait (void)
{
  this->http_request.disconnect();
}

/* ---------------------------------------------------------------- */

void
GtkPortrait::set (std::string const& charid)
{
  this->char_id = charid;

  bool success = false;

  /* Try to get the portrait from our own cache. */
  success = this->fetch_from_gtkevemon_cache();

  /* Use default image and request online. */
  if (!success)
  {
    Glib::RefPtr<Gdk::Pixbuf> pixbuf = ImageStore::eveportrait;
    Glib::RefPtr<Gdk::Pixbuf> scaled = pixbuf->scale_simple
        (PORTRAIT_SIZE, PORTRAIT_SIZE, Gdk::INTERP_BILINEAR);
    this->Gtk::Image::set(scaled);
    this->request_from_eve_online();
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

void
GtkPortrait::request_from_eve_online (void)
{
  std::cout << "Requesting portrait: " << this->char_id
      << " ..." << std::endl;

  AsyncHttp* http = AsyncHttp::create();;
  http->set_host("img.eve.is");
  http->set_path("/serv.asp?s=256&c=" + this->char_id);
  http->set_agent("GtkEveMon");

  this->http_request.disconnect();
  this->http_request = http->signal_done().connect(sigc::mem_fun
      (*this, &GtkPortrait::set_from_eve_online));

  http->async_request();
}

/* ---------------------------------------------------------------- */

void
GtkPortrait::set_from_eve_online (AsyncHttpData result)
{
  if (result.data.get() == 0)
  {
    std::cout << "Error fetching portrait from EVE Online!" << std::endl;
    return;
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
    out.write(result.data->data, result.data->size);
    out.close();

    Glib::RefPtr<Gdk::Pixbuf> image
        = Gdk::Pixbuf::create_from_file(jpg_name.str())
        ->scale_simple(PORTRAIT_SIZE, PORTRAIT_SIZE, Gdk::INTERP_BILINEAR);

    this->cache_portrait(image);
    ::unlink(jpg_name.str().c_str());
    this->fetch_from_gtkevemon_cache();
  }
  catch (...)
  {
    std::cout << "Error saving portrait from EVE Online" << std::endl;
    return;
  }
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
