#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <gtkmm/messagedialog.h>

#include "http.h"
#include "imagestore.h"
#include "config.h"
#include "gtkportrait.h"

GtkPortrait::GtkPortrait (void)
{
  this->add(this->image);
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
    this->image.set(scaled);
    this->request_from_eve_online();
  }
}

/* ---------------------------------------------------------------- */

void
GtkPortrait::set_enable_clicks (void)
{
  this->signal_button_press_event().connect(sigc::mem_fun
      (*this, &GtkPortrait::on_button_press_myevent));
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

    Glib::RefPtr<Gdk::Pixbuf> portrait = GtkPortrait::create_from_file
        (filename.str());
    this->image.set(portrait);
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
  std::stringstream jpg_name;
  jpg_name << confdir << "/" << this->char_id << "_256.jpg";

  try
  {
    std::ofstream out(jpg_name.str().c_str());
    out.write(result.data->data, result.data->size);
    out.close();

    Glib::RefPtr<Gdk::Pixbuf> image
        = GtkPortrait::create_from_file(jpg_name.str())
        ->scale_simple(PORTRAIT_SIZE, PORTRAIT_SIZE, Gdk::INTERP_BILINEAR);

    this->cache_portrait(image);
    ::unlink(jpg_name.str().c_str());
    this->fetch_from_gtkevemon_cache();
  }
  catch (...)
  {
    ::unlink(jpg_name.str().c_str());
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
    /* Ignore errors. We'll get them on creation. */
    ::mkdir(portraitdir.c_str(), S_IRWXU);
  }

  try
  {
    std::string filename = this->get_portrait_file();
    GtkPortrait::save_to_file(portrait, filename);
  }
  catch (...)
  {
    std::cout << "Error caching portrait for " << this->char_id << std::endl;
    return;
  }

  std::cout << "Cached portrait: " << this->char_id << std::endl;
}

/* ---------------------------------------------------------------- */

bool
GtkPortrait::on_button_press_myevent (GdkEventButton* event)
{
  event = 0;
  this->request_from_eve_online();

  Gtk::Window* toplevel = (Gtk::Window*)this->get_toplevel();
  Gtk::MessageDialog md("Portrait has been re-requested!",
      false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
  md.set_secondary_text("A new portrait has been requested from the "
      "EVE image server. If you can only see a \"!\" as portrait, "
      "the image server most likely does not have the portrait. This "
      "happens to new players or after a portrait server reset.\n\n"
      "If you can see a \"!\" and the text \"GtkEveMon fallback\", the "
      "portrait couldn't be requested and a local fallback is used.\n\n"
      "Image URL: http://img.eve.is/serv.asp?s=256&c=" + this->char_id);
  md.set_title("Portrait re-request - GtkEveMon");
  md.set_transient_for(*toplevel);
  md.run();

  return true;
}

/* ---------------------------------------------------------------- */

std::string
GtkPortrait::get_portrait_dir (void)
{
  return Config::get_conf_dir() + "/portraits";
}

/* ---------------------------------------------------------------- */

std::string
GtkPortrait::get_portrait_file (void)
{
  std::stringstream filename;
  filename << this->get_portrait_dir();
  filename << "/" << this->char_id << "_" << PORTRAIT_SIZE << ".png";
  return filename.str();
}

/* ---------------------------------------------------------------- */

Glib::RefPtr<Gdk::Pixbuf>
GtkPortrait::create_from_file (std::string const& fn)
{
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  return Gdk::Pixbuf::create_from_file(fn);
  #else
  std::auto_ptr<Glib::Error> error;
  Glib::RefPtr<Gdk::Pixbuf> ret = Gdk::Pixbuf::create_from_file(fn, error);
  if (error.get())
    throw error;
  return ret;
  #endif
}

/* ---------------------------------------------------------------- */

void
GtkPortrait::save_to_file (Glib::RefPtr<Gdk::Pixbuf> pixbuf,
    std::string const& fn)
{
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  pixbuf->save(fn, "png");
  #else
  std::auto_ptr<Glib::Error> error;
  pixbuf->save(fn, "png", error);
  if (error.get())
    throw error;
  #endif
}
