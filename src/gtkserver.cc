#include <iostream>
#include <gtkmm/stock.h>
#include <gtkmm/label.h>

#include "exception.h"
#include "serverlist.h"
#include "helpers.h"
#include "thread.h"
#include "gtkserver.h"

class GtkServerChecker : public Thread
{
  private:
    Server* server;

  protected:
    void* run (void);

  public:
    GtkServerChecker (Server* server);
};

/* ---------------------------------------------------------------- */

GtkServerChecker::GtkServerChecker (Server* server)
{
  this->server = server;
}

/* ---------------------------------------------------------------- */

void*
GtkServerChecker::run (void)
{
  try
  {
    this->server->refresh();
  }
  catch (Exception& e)
  {
    std::cout << "Error refeshing server: " << e << std::endl;
  }

  delete this;
  return 0;
}

/* ================================================================ */

GtkServer::GtkServer (Server* server)
  : Gtk::Table(2, 3, false)
{
  this->server = server;
  this->status_desc.set_text("Status:");
  this->status.set_text("Fetching");

  this->set_col_spacings(5);
  this->set_status_icon(Gtk::Stock::REFRESH);
  this->status_but.set_relief(Gtk::RELIEF_NONE);

  Gtk::Label* label_server = Gtk::manage(new Gtk::Label("Server:"));

  label_server->set_alignment(Gtk::ALIGN_LEFT);
  this->status_desc.set_alignment(Gtk::ALIGN_LEFT);

  this->name.set_alignment(Gtk::ALIGN_RIGHT);
  this->status.set_alignment(Gtk::ALIGN_RIGHT);

  this->name.set_text(server->get_name());

  this->attach(this->status_but, 0, 1, 0, 2, Gtk::FILL, Gtk::FILL);
  this->attach(*label_server, 1, 2, 0, 1, Gtk::FILL, Gtk::FILL);
  this->attach(this->status_desc, 1, 2, 1, 2, Gtk::FILL, Gtk::FILL);
  this->attach(this->name, 2, 3, 0, 1, Gtk::FILL, Gtk::FILL);
  this->attach(this->status, 2, 3, 1, 2, Gtk::FILL, Gtk::FILL);

  this->status_but.signal_clicked().connect
      (sigc::mem_fun(*this, &GtkServer::force_refresh));
  this->server->signal_updated().connect
      (sigc::mem_fun(*this, &GtkServer::update));

  this->update();
  this->show_all();
}

/* ---------------------------------------------------------------- */

void
GtkServer::update (void)
{
  if (server->is_refreshing() || server->get_players() == -1)
  {
    this->set_status_icon(Gtk::Stock::REFRESH);
  }
  else if (server->is_online())
  {
    this->set_status_icon(Gtk::Stock::YES);
    this->status_desc.set_text("Players:");
    this->status.set_text(Helpers::get_dotted_str_from_int
        (this->server->get_players()));
  }
  else
  {
    this->set_status_icon(Gtk::Stock::NO);
    this->status_desc.set_text("Status:");
    this->status.set_text("Offline");
  }
}

/* ---------------------------------------------------------------- */

void
GtkServer::force_refresh (void)
{
  GtkServerChecker* sc = new GtkServerChecker(this->server);
  sc->pt_create();
  this->set_status_icon(Gtk::Stock::REFRESH);
}

/* ---------------------------------------------------------------- */

void
GtkServer::set_status_icon (Gtk::BuiltinStockID const& id)
{
  this->status_but.set_image(*Gtk::manage
      (new Gtk::Image(id, Gtk::ICON_SIZE_BUTTON)));
}
