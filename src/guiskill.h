#ifndef GUI_SKILL_HEADER
#define GUI_SKILL_HEADER

#include <gtkmm/label.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/textview.h>

#include "winbase.h"

class GuiSkill : public WinBase
{
  private:
    Gtk::Label skill_name;
    Gtk::Label group_name;
    Gtk::Label skill_attribs;
    Glib::RefPtr<Gtk::TextBuffer> desc_buffer;
    Gtk::TextView skill_desc;

  public:
    GuiSkill (void);
    void set_skill (int skill_id);
};

#endif /* GUI_SKILL_HEADER */
