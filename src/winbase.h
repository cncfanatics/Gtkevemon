#ifndef WINBASE_HEADER
#define WINBASE_HEADER

#include <gtkmm/window.h>

class WinBase : public Gtk::Window
{
  protected:
    virtual bool on_delete_event (GdkEventAny* event);

  public:
    virtual ~WinBase (void);
    virtual void close (void);
};

/* ---------------------------------------------------------------- */

inline
WinBase::~WinBase (void)
{
}

inline bool
WinBase::on_delete_event (GdkEventAny* event)
{
  event = 0;
  this->close();
  return false;
}

inline void
WinBase::close (void)
{
  delete this;
}

#endif /* WINBASE_HEADER */
