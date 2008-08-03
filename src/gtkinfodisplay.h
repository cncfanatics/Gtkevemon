#ifndef GTK_INFO_DISPLAY
#define GTK_INFO_DISPLAY

#include <gtkmm/label.h>
#include <gtkmm/image.h>
#include <gtkmm/box.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/textview.h>
#include <gtkmm/tooltips.h>

#include "winbase.h"

enum InfoItemType
{
  INFO_NOTIFICATION,
  INFO_WARNING,
  INFO_ERROR
};

enum InfoDisplayStyle
{
  INFO_STYLE_NONE,
  INFO_STYLE_TOP_HSEP,
  INFO_STYLE_FRAMED
};

/* ---------------------------------------------------------------- */

class InfoItem
{
  public:
    InfoItemType type;
    std::string message;
    std::string details;

    InfoItem (void);
    InfoItem (InfoItemType type, std::string const& message);
    InfoItem (InfoItemType type, std::string const& message,
        std::string const& details);
};

/* ---------------------------------------------------------------- */

class GtkInfoDisplay : public Gtk::VBox
{
  private:
    std::vector<InfoItem> log;
    Gtk::Image icon;
    Gtk::Label text;
    Gtk::Button info_but;
    Gtk::Tooltips tooltips;

  protected:
    void show_info_log (void);

  public:
    GtkInfoDisplay (InfoDisplayStyle style);

    void append (InfoItemType type, std::string const& message);
    void append (InfoItemType type, std::string const& message,
        std::string const& details);
    void append (InfoItem const& item);
};

/* ---------------------------------------------------------------- */

class GtkInfoDisplayLog : public WinBase
{
  private:
    Gtk::Label message;
    Glib::RefPtr<Gtk::TextBuffer> text_buffer;
    Gtk::TextView text_view;

  public:
    GtkInfoDisplayLog (InfoItem const& item);
};

/* ---------------------------------------------------------------- */

inline
InfoItem::InfoItem (void)
{
  this->type = INFO_ERROR;
}

inline
InfoItem::InfoItem (InfoItemType type, std::string const& message)
{
  this->type = type;
  this->message = message;
}

inline
InfoItem::InfoItem (InfoItemType type, std::string const& message,
    std::string const& details)
{
  this->type = type;
  this->message = message;
  this->details = details;
}

inline void
GtkInfoDisplay::append (InfoItemType type, std::string const& message)
{
  this->append(InfoItem(type, message));
}

inline void
GtkInfoDisplay::append (InfoItemType type, std::string const& message,
    std::string const& details)
{
  this->append(InfoItem(type, message, details));
}

#endif /* GTK_INFO_DISPLAY */
