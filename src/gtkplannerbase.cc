#include <iostream>

#include <gtkmm/image.h>
#include <gtkmm/imagemenuitem.h>

#include "imagestore.h"
#include "helpers.h"
#include "gtkplannerbase.h"

GtkSkillContextMenu::GtkSkillContextMenu (void)
{
  this->signal_selection_done().connect(sigc::mem_fun
      (*this, &GtkSkillContextMenu::delete_me));
}

/* ---------------------------------------------------------------- */

void
GtkSkillContextMenu::delete_me (void)
{
  delete this;
}

/* ---------------------------------------------------------------- */

void
GtkSkillContextMenu::set_skill (ApiSkill const* skill, int min_level)
{
  this->skill = skill;

  Gtk::ImageMenuItem* to_level[5];
  for (unsigned int i = 0; i < 5; ++i)
  {
    Glib::RefPtr<Gdk::Pixbuf> icon;
    if ((int)i + 1 <= min_level)
      icon = ImageStore::skillicons[5];
    else
      icon = ImageStore::skillicons[1];

    to_level[i] = Gtk::manage(new Gtk::ImageMenuItem
        (*Gtk::manage(new Gtk::Image(icon)),
        "Train to level " + Helpers::get_string_from_uint(i + 1)));
    to_level[i]->signal_activate().connect(sigc::bind(sigc::mem_fun
        (*this, &GtkSkillContextMenu::on_training_requested), i + 1));
    this->append(*to_level[i]);
  }
  this->show_all();
}

/* ---------------------------------------------------------------- */

void
GtkSkillContextMenu::on_training_requested (int level)
{
  this->sig_planning_requested.emit(this->skill, level);
}

/* ---------------------------------------------------------------- */

sigc::signal<void, ApiSkill const*, int>&
GtkSkillContextMenu::signal_planning_requested (void)
{
  return sig_planning_requested;
}

/* ================================================================ */

bool
GtkSkillListView::on_button_press_event (GdkEventButton* event)
{
  bool return_value = this->Gtk::TreeView::on_button_press_event(event);
  this->sig_button_press_event.emit(event);
  return return_value;
}

