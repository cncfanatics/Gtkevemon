#include <gtkmm/stock.h>
#include <gtkmm/button.h>
#include <gtkmm/separator.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/table.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/frame.h>

#include "apiskilltree.h"
#include "helpers.h"
#include "gtkdefines.h"
#include "guiskill.h"

GuiSkill::GuiSkill (void)
  : desc_buffer(Gtk::TextBuffer::create()), skill_desc(desc_buffer)
{
  this->skill_desc.set_wrap_mode(Gtk::WRAP_WORD);
  this->skill_desc.set_editable(false);
  this->skill_name.set_selectable(true);
  this->skill_name.property_xalign() = 0.0f;
  this->group_name.set_selectable(true);
  this->group_name.property_xalign() = 0.0f;
  this->skill_attribs.set_selectable(true);
  this->skill_attribs.property_xalign() = 0.0f;

  Gtk::Label* skill_name_desc = MK_LABEL("Skill name:");
  Gtk::Label* group_name_desc = MK_LABEL("Skill group:");
  Gtk::Label* skill_attribs_desc = MK_LABEL("Attributes:");
  skill_name_desc->property_xalign() = 0.0f;
  group_name_desc->property_xalign() = 0.0f;
  skill_attribs_desc->property_xalign() = 0.0f;

  Gtk::Table* info_table = Gtk::manage(new Gtk::Table(3, 2, false));
  info_table->set_col_spacings(5);
  info_table->set_row_spacings(5);
  info_table->attach(*skill_name_desc, 0, 1, 0, 1, Gtk::FILL, Gtk::SHRINK);
  info_table->attach(*group_name_desc, 0, 1, 1, 2, Gtk::FILL, Gtk::SHRINK);
  info_table->attach(*skill_attribs_desc, 0, 1, 2, 3, Gtk::FILL, Gtk::SHRINK);
  info_table->attach(this->skill_name, 1, 2, 0, 1,
      Gtk::FILL|Gtk::EXPAND, Gtk::SHRINK);
  info_table->attach(this->group_name, 1, 2, 1, 2,
      Gtk::FILL|Gtk::EXPAND, Gtk::SHRINK);
  info_table->attach(this->skill_attribs, 1, 2, 2, 3,
      Gtk::FILL|Gtk::EXPAND, Gtk::SHRINK);

  Gtk::HBox* desc_separator = MK_HBOX;
  desc_separator->pack_start(*MK_HSEP, true, true, 0);
  desc_separator->pack_start(*MK_LABEL("Description"), false, false, 0);
  desc_separator->pack_start(*MK_HSEP, true, true, 0);

  Gtk::ScrolledWindow* desc_scwin = Gtk::manage(new Gtk::ScrolledWindow);
  desc_scwin->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  desc_scwin->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
  desc_scwin->add(this->skill_desc);

  Gtk::VBox* skill_vbox = MK_VBOX;
  skill_vbox->set_border_width(5);
  skill_vbox->pack_start(*info_table, false, false, 0);
  skill_vbox->pack_start(*desc_separator, false, false, 0);
  skill_vbox->pack_start(*desc_scwin, true, true, 0);

  Gtk::Frame* info_frame = MK_FRAME0;
  info_frame->set_shadow_type(Gtk::SHADOW_OUT);
  info_frame->add(*skill_vbox);

  Gtk::Button* close_but = MK_BUT(Gtk::Stock::CLOSE);
  Gtk::HBox* button_box = MK_HBOX;
  button_box->pack_start(*MK_HSEP, true, true, 0);
  button_box->pack_start(*close_but, false, false, 0);

  Gtk::VBox* main_vbox = MK_VBOX;
  main_vbox->set_border_width(5);
  main_vbox->pack_start(*info_frame, true, true, 0);
  main_vbox->pack_end(*button_box, false, false, 0);

  close_but->signal_clicked().connect(sigc::mem_fun(*this, &WinBase::close));

  this->add(*main_vbox);
  this->set_title("Skill Details");
  this->set_default_size(350, 300);
  this->show_all();
}

/* ---------------------------------------------------------------- */

void
GuiSkill::set_skill (int skill_id)
{
  this->skill_name.set_text("---");
  this->group_name.set_text("---");

  try
  {
    ApiSkillTreePtr tree = ApiSkillTree::request();
    ApiSkill const* skill = tree->get_skill_for_id(skill_id);
    int group_id = skill->group;
    ApiSkillGroup const* group = tree->get_group_for_id(group_id);

    this->skill_name.set_text(skill->name + " ("
        + Helpers::get_string_from_int(skill->rank) + ")");
    this->group_name.set_text(group->name);
    this->skill_attribs.set_text(ApiSkillTree::get_attrib_name(skill->primary)
        + std::string(" / ") + ApiSkillTree::get_attrib_name(skill->secondary));
    this->desc_buffer->set_text(skill->desc);
  }
  catch (...)
  {
    this->skill_name.set_text(Helpers::get_string_from_int(skill_id));
  }
}
