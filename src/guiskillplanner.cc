#include <iostream>
#include <gtkmm/table.h>
#include <gtkmm/main.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/stock.h>
#include <gtkmm/image.h>
#include <gtkmm/paned.h>
#include <gtkmm/separator.h>
#include <gtkmm/scrolledwindow.h>

#include "helpers.h"
#include "imagestore.h"
#include "gtkportrait.h"
#include "gtkdefines.h"
#include "guiskillplanner.h"

GuiSkillPlanner::GuiSkillPlanner (void)
{
  this->skill_browser.set_border_width(5);
  this->cert_browser.set_border_width(5);

  this->browser_nb.append_page(this->skill_browser, "Skills");
  this->browser_nb.append_page(this->cert_browser, "Certificates");
  this->browser_nb.set_size_request(260, -1);

  this->details_nb.append_page(this->plan_gui, "Training plan");
  this->details_nb.append_page(this->details_gui, "Item details");

  Gtk::Button* close_but = MK_BUT(Gtk::Stock::CLOSE);
  Gtk::HBox* button_hbox = MK_HBOX;
  button_hbox->pack_start(*MK_HSEP, true, true, 0);
  button_hbox->pack_start(*close_but, false, false, 0);

  Gtk::VBox* details_panechild = MK_VBOX;
  details_panechild->pack_start(this->details_nb, true, true, 0);
  details_panechild->pack_start(*button_hbox, false, false, 0);

  Gtk::HPaned* main_pane = MK_HPANED;
  main_pane->add1(this->browser_nb);
  main_pane->add2(*details_panechild);

  Gtk::VBox* main_vbox = MK_VBOX;
  main_vbox->set_border_width(5);
  main_vbox->pack_start(*main_pane, true, true, 0);

  /* Signals. */
  this->skill_browser.signal_element_selected().connect
      (sigc::mem_fun(*this, &GuiSkillPlanner::on_element_selected));
  this->skill_browser.signal_element_activated().connect
      (sigc::mem_fun(*this, &GuiSkillPlanner::on_element_activated));
  this->skill_browser.signal_planning_requested().connect
      (sigc::mem_fun(*this, &GuiSkillPlanner::on_planning_requested));
  this->cert_browser.signal_element_selected().connect
      (sigc::mem_fun(*this, &GuiSkillPlanner::on_element_selected));
  this->cert_browser.signal_element_activated().connect
      (sigc::mem_fun(*this, &GuiSkillPlanner::on_element_activated));

  close_but->signal_clicked().connect(sigc::mem_fun(*this, &WinBase::close));
  this->plan_gui.signal_skill_activated().connect(sigc::mem_fun
      (*this, &GuiSkillPlanner::on_element_activated));
  this->details_gui.signal_planning_requested().connect(sigc::mem_fun
      (*this, &GuiSkillPlanner::on_planning_requested));
  Gtk::Main::signal_quit().connect(sigc::mem_fun
      (*this, &GuiSkillPlanner::on_gtkmain_quit));

  this->add(*main_vbox);
  this->set_title("Skill browser - GtkEveMon");
  this->set_icon(ImageStore::applogo);
  this->set_default_size(800, 550);
  this->show_all();
}

/* ---------------------------------------------------------------- */

GuiSkillPlanner::~GuiSkillPlanner (void)
{
}

/* ---------------------------------------------------------------- */

void
GuiSkillPlanner::set_character (ApiCharSheetPtr sheet)
{
  this->charsheet = sheet;
  this->details_gui.set_character(sheet);
  this->plan_gui.set_character(sheet);
  this->skill_browser.set_character(sheet);
  this->cert_browser.set_character(sheet);
  this->set_title(this->charsheet->name + " - GtkEveMon");
}

/* ---------------------------------------------------------------- */

void
GuiSkillPlanner::set_training (ApiInTrainingPtr training)
{
  this->training = training;
  this->plan_gui.set_training(training);
}

/* ---------------------------------------------------------------- */

void
GuiSkillPlanner::on_element_selected (ApiElement const* elem)
{
  this->details_gui.set_element(elem);
}

/* ---------------------------------------------------------------- */

void
GuiSkillPlanner::on_element_activated (ApiElement const* elem)
{
  this->details_nb.set_current_page(1);
  this->details_gui.set_element(elem);
}

/* ---------------------------------------------------------------- */

void
GuiSkillPlanner::on_planning_requested (ApiSkill const* skill, int level)
{
  this->details_nb.set_current_page(0);
  this->plan_gui.append_skill(skill, level);
}

/* ---------------------------------------------------------------- */

bool
GuiSkillPlanner::on_gtkmain_quit (void)
{
  this->close();
  return false;
}
