#include <iostream>
#include <gtkmm/stock.h>
#include <gtkmm/label.h>

#include "exception.h"
#include "serverlist.h"
#include "helpers.h"
#include "gtkserver.h"

GtkServer::GtkServer (Server& server)
  : Gtk::Table(4, 3, false)
{
  this->server = &server;

  this->set_col_spacings(5);
  this->set_status_icon(Gtk::Stock::REFRESH);
  this->status_but.set_relief(Gtk::RELIEF_NONE);

  Gtk::Label* label_server = Gtk::manage(new Gtk::Label("Server:"));
  Gtk::Label* label_status = Gtk::manage(new Gtk::Label("Status:"));
  Gtk::Label* label_population = Gtk::manage(new Gtk::Label("Population:"));
  Gtk::Label* label_address = Gtk::manage(new Gtk::Label("Address:"));

  label_server->property_xalign() = 0.0f;
  label_status->property_xalign() = 0.0f;
  label_population->property_xalign() = 0.0f;
  label_address->property_xalign() = 0.0f;

  this->name.property_xalign() = 1.0f;
  this->status.property_xalign() = 1.0f;
  this->population.property_xalign() = 1.0f;
  this->address.property_xalign() = 1.0f;

  this->name.set_text(server.get_name());
  this->address.set_text(server.get_host());

  this->attach(this->status_but, 0, 1, 0, 2, Gtk::FILL, Gtk::FILL);
  this->attach(*label_server, 1, 2, 0, 1, Gtk::FILL, Gtk::FILL);
  this->attach(*label_status, 1, 2, 1, 2, Gtk::FILL, Gtk::FILL);
  this->attach(*label_population, 0, 2, 2, 3, Gtk::FILL, Gtk::FILL);
  this->attach(*label_address, 0, 2, 3, 4, Gtk::FILL, Gtk::FILL);
  this->attach(this->name, 2, 3, 0, 1, Gtk::FILL, Gtk::FILL);
  this->attach(this->status, 2, 3, 1, 2, Gtk::FILL, Gtk::FILL);
  this->attach(this->population, 2, 3, 2, 3, Gtk::FILL, Gtk::FILL);
  this->attach(this->address, 2, 3, 3, 4, Gtk::FILL, Gtk::FILL);

  this->status_but.signal_clicked().connect
      (sigc::mem_fun(*this, &GtkServer::force_refresh));

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
    this->status.set_text("Online");
    this->population.set_text(Helpers::get_dotted_str_from_int
        (this->server->get_players()));
  }
  else
  {
    this->set_status_icon(Gtk::Stock::NO);
    this->status.set_text("Offline");
    this->population.set_text(Helpers::get_dotted_str_from_int
        (this->server->get_players()));
  }
}

/* ---------------------------------------------------------------- */

void
GtkServer::force_refresh (void)
{
  try
  {
    this->server->refresh();
  }
  catch (Exception& e)
  {
    std::cout << "Error refeshing server: " << e << std::endl;
  }

  this->update();
}

/* ---------------------------------------------------------------- */

void
GtkServer::set_status_icon (Gtk::BuiltinStockID const& id)
{
  this->status_but.set_image(*Gtk::manage
      (new Gtk::Image(id, Gtk::ICON_SIZE_BUTTON)));
}
