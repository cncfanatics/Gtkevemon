#ifndef GTK_SERVER_HEADER
#define GTK_SERVER_HEADER

#include <gtkmm/table.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>

#include "server.h"

class GtkServer : public Gtk::Table
{
  private:
    Server* server;

    Gtk::Button status_but;
    Gtk::Label name;
    Gtk::Label status;
    Gtk::Label population;
    Gtk::Label address;

    void force_refresh (void);
    void set_status_icon (Gtk::BuiltinStockID const& id);

  public:
    GtkServer (Server& server);
    void update (void);
};

#endif /* GTK_SERVER_HEADER */
