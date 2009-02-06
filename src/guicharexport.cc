#include <iostream>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/separator.h>
#include <gtkmm/label.h>
#include <gtkmm/textview.h>
#include <gtkmm/stock.h>

#include "helpers.h"
#include "gtkdefines.h"
#include "guicharexport.h"

GuiCharExport::GuiCharExport (ApiCharSheetPtr charsheet)
  : text_buf(Gtk::TextBuffer::create())
{
  Gtk::TextView* text_view = Gtk::manage(new Gtk::TextView(this->text_buf));
  text_view->set_editable(false);

  Gtk::ScrolledWindow* scwin = MK_SCWIN;
  scwin->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
  scwin->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  scwin->add(*text_view);

  Gtk::Button* close_but = MK_BUT(Gtk::Stock::CLOSE);
  Gtk::HBox* button_box = MK_HBOX;
  button_box->pack_start(*MK_HSEP, true, true, 0);
  button_box->pack_start(*close_but, false, false, 0);

  Gtk::Label* info_label = MK_LABEL("Currently you can only export "
      "the character information in BB-code. If you need other export "
      "formats, you can make a feature request.");
  info_label->set_line_wrap(true);
  info_label->set_width_chars(60);
  info_label->set_alignment(Gtk::ALIGN_LEFT);

  Gtk::VBox* main_box = MK_VBOX;
  main_box->set_border_width(5);
  main_box->pack_start(*info_label, false, false, 0);
  main_box->pack_start(*scwin, true, true, 0);
  main_box->pack_start(*button_box, false, false, 0);

  close_but->signal_clicked().connect(sigc::mem_fun(*this, &WinBase::close));

  this->add(*main_box);
  this->set_title("Character Information - GtkEveMon");
  this->set_default_size(500, 600);
  this->show_all();

  this->fill_textbuffer(charsheet);
  this->text_buf->select_range(this->text_buf->begin(),
      this->text_buf->begin());
}

/* ---------------------------------------------------------------- */

void
GuiCharExport::fill_textbuffer (ApiCharSheetPtr charsheet)
{
  this->append_buf("[b]" + charsheet->name + "[/b]\n\n");

  this->append_buf("[b]Attributes[/b]\n");
  this->append_buf("Intelligence: "
      + Helpers::get_string_from_double(charsheet->total.intl, 2) + "\n");
  this->append_buf("Perception: "
      + Helpers::get_string_from_double(charsheet->total.per, 2) + "\n");
  this->append_buf("Charisma: "
      + Helpers::get_string_from_double(charsheet->total.cha, 2) + "\n");
  this->append_buf("Willpower: "
      + Helpers::get_string_from_double(charsheet->total.wil, 2) + "\n");
  this->append_buf("Memory: "
      + Helpers::get_string_from_double(charsheet->total.mem, 2) + "\n\n");

  /* Create the data structure. */
  GroupMap data;
  GuiCharExport::create_datamap(data, charsheet);

  /* Make a list with amount of skills per level. */
  std::vector<unsigned int> skills_at;
  skills_at.resize(6, 0);

  /* Iterate over all skill groups. */
  unsigned int grandtotal = 0;
  for (GroupMap::iterator iter = data.begin(); iter != data.end(); iter++)
  {
    this->append_buf("[b]" + iter->first + "[/b]\n");

    /* Iterate over skills in this skill group. */
    unsigned int total = 0;
    for (SkillMap::iterator siter = iter->second.begin();
        siter != iter->second.end(); siter++)
    {
      this->append_buf("[img]http://myeve.eve-online.com/bitmaps/character/"
          "level" + Helpers::get_string_from_uint(siter->second->level)
          + ".gif[/img] " + siter->first + "\n");

      /* Count total skill points for the group. */
      total += siter->second->points;

      /* Count skill for the skill level. */
      skills_at[siter->second->level] += 1;
    }

    grandtotal += total;
    this->append_buf("Total Skillpoints in Group: "
        + Helpers::get_dotted_str_from_uint(total) + "\n\n");
  }

  /* Misc info */
  this->append_buf("Total Skillpoints: "
      + Helpers::get_dotted_str_from_uint(grandtotal) + "\n");
  this->append_buf("Total Number of Skills: "
      + Helpers::get_string_from_uint(charsheet->skills.size()) + "\n\n");

  for (unsigned int i = 0; i < skills_at.size(); ++i)
    this->append_buf("Skills at Level " + Helpers::get_string_from_int(i)
        + ": " + Helpers::get_string_from_int(skills_at[i]) + "\n");
}

/* ---------------------------------------------------------------- */

void
GuiCharExport::append_buf (Glib::ustring const& text)
{
  this->text_buf->insert(this->text_buf->end(), text);
}

/* ---------------------------------------------------------------- */

void
GuiCharExport::create_datamap (GroupMap& result, ApiCharSheetPtr charsheet)
{
  ApiSkillTreePtr tree = ApiSkillTree::request();

  for (unsigned int i = 0; i < charsheet->skills.size(); ++i)
  {
    ApiSkill const* skill = charsheet->skills[i].details;
    ApiSkillGroup const* group = tree->get_group_for_id(skill->group);
    if (group == 0)
    {
      std::cout << "Warning: Group " << skill->group
          << " does not exist!" << std::endl;
      continue;
    }

    std::string groupname = group->name;
    result[groupname][skill->name] = &charsheet->skills[i];
  }
}
