#include <string>
#include <sstream>
#include <gtkmm/image.h>

#include "evetime.h"
#include "imagestore.h"
#include "helpers.h"
#include "gtkhelpers.h"

void
GtkHelpers::create_tooltip (Glib::RefPtr<Gtk::Tooltip> const& tooltip,
        ApiSkill const* skill, ApiCharSheetSkill* cskill,
        ApiCharSheetPtr sheet)
{
  tooltip->set_icon(ImageStore::skill);

  std::stringstream ss;
  ss << "Name: " << skill->name << "\n"
      << "Attributes: " << ApiSkillTree::get_attrib_name(skill->primary)
      << " / " << ApiSkillTree::get_attrib_name(skill->secondary) << "\n";

  if (cskill != 0 && sheet.get() != 0 && sheet->valid && cskill->level != 5)
  {
    int sp_to_go = cskill->points_dest - cskill->points;
    double sppm = sheet->get_sppm_for_skill(skill);
    time_t secs_next_level = (time_t)(60.0 * (double)sp_to_go / sppm);
    std::string next_level_str = EveTime::get_string_for_timediff
        (secs_next_level, false);
    if (cskill->completed != 0.0)
      next_level_str += " (" + Helpers::get_string_from_double
          (cskill->completed * 100.0, 0) + "% completed)";
    ss << "SP per hour: " << (int)(sppm * 60.0) << "\n"
        << "Destination SP: "
        << Helpers::get_dotted_str_from_int(cskill->points_dest) << "\n"
        << "Training time: " << next_level_str << "\n";
  }

  ss  << "\n" << skill->desc;
  tooltip->set_text(ss.str());
}

/* ---------------------------------------------------------------- */

std::string
GtkHelpers::locale_to_utf8 (Glib::ustring const& opsys_string)
{
  /* We don't throw the error further away, we'll handle it here. */
  #define LOCALE_TO_UTF8_ERROR "Error converting string to UTF-8!"
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  try
  { return Glib::locale_to_utf8(opsys_string); }
  catch (...)
  { return LOCALE_TO_UTF8_ERROR; }
  #else
  std::auto_ptr<Glib::Error> error;
  std::string ret = Glib::locale_to_utf8(opsys_string, error);
  if (error)
    return LOCALE_TO_UTF8_ERROR;
  return ret;
  #endif
}
