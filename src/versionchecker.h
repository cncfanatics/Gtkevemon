#ifndef VERSION_CHECKER_HEADER
#define VERSION_CHECKER_HEADER

#include <gtkmm/window.h>

#include "asynchttp.h"

/* This is the amount of milli seconds between version checks. */
#define VERSION_CHECK_INTERVAL 3600000

class VersionChecker
{
  private:
    Gtk::Window* parent_win;
    sigc::connection request_conn;

    void handle_result (AsyncHttpData result);

  public:
    VersionChecker (void);
    ~VersionChecker (void);

    void set_parent_window (Gtk::Window* parent);
    bool request_version (void);
};

/* ---------------------------------------------------------------- */

inline void
VersionChecker::set_parent_window (Gtk::Window* parent)
{
  this->parent_win = parent;
}

#endif /* VERSION_CHECKER_HEADER */
