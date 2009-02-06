/*
 * This file is part of GtkEveMon.
 *
 * GtkEveMon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with GtkEveMon. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GUI_CHAR_EXPORT_HEADER
#define GUI_CHAR_EXPORT_HEADER

#include <map>
#include <string>
#include <gtkmm/textbuffer.h>

#include "apicharsheet.h"
#include "winbase.h"

class GuiCharExport : public WinBase
{
  public:
    typedef std::map<std::string, ApiCharSheetSkill const*> SkillMap;
    typedef std::map<std::string, SkillMap> GroupMap;

  private:
    Glib::RefPtr<Gtk::TextBuffer> text_buf;

  protected:
    void append_buf (Glib::ustring const& text);
    void fill_textbuffer (ApiCharSheetPtr charsheet);

  public:
    GuiCharExport (ApiCharSheetPtr charsheet);

    static void create_datamap (GroupMap& result, ApiCharSheetPtr charsheet);
};

#endif /* GUI_CHAR_EXPORT_HEADER */
