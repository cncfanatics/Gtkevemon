#include <iostream>
#include <gtkmm/stock.h>
#include <gtkmm/scrolledwindow.h>

#include "helpers.h"
#include "gtkhelpers.h"
#include "gtkdefines.h"
#include "imagestore.h"
#include "gtkitembrowser.h"

ItemBrowserBase::ItemBrowserBase (void)
  : store(Gtk::TreeStore::create(cols)),
    view(store)
{
  Gtk::TreeViewColumn* col_name = Gtk::manage(new Gtk::TreeViewColumn);
  col_name->set_title("Name");
  col_name->pack_start(this->cols.icon, false);
  col_name->pack_start(this->cols.name, true);
  this->view.append_column(*col_name);
  this->view.set_headers_visible(false);
  this->view.get_selection()->set_mode(Gtk::SELECTION_SINGLE);

  this->view.get_selection()->signal_changed().connect
      (sigc::mem_fun(*this, &ItemBrowserBase::on_selection_changed));
  this->view.signal_row_activated().connect(sigc::mem_fun
      (*this, &ItemBrowserBase::on_row_activated));
  this->view.signal_button_press_myevent().connect(sigc::mem_fun
      (*this, &ItemBrowserBase::on_view_button_pressed));
  this->view.signal_query_tooltip().connect(sigc::mem_fun
      (*this, &ItemBrowserBase::on_query_element_tooltip));
  this->view.set_has_tooltip(true);
}

/* ---------------------------------------------------------------- */

void
ItemBrowserBase::on_selection_changed (void)
{
  if (this->view.get_selection()->get_selected_rows().empty())
    return;

  Gtk::TreeModel::iterator iter = this->view.get_selection()->get_selected();

  ApiElement const* elem = (*iter)[this->cols.data];
  if (elem == 0)
    return;

  this->sig_element_selected.emit(elem);
}

/* ---------------------------------------------------------------- */

void
ItemBrowserBase::on_row_activated (Gtk::TreeModel::Path const& path,
    Gtk::TreeViewColumn* col)
{
  col = 0;

  Gtk::TreeModel::iterator iter = this->store->get_iter(path);
  ApiElement const* elem = (*iter)[this->cols.data];

  if (elem != 0)
  {
    this->sig_element_activated.emit(elem);
  }
  else
  {
    if (this->view.row_expanded(path))
      this->view.collapse_row(path);
    else
      this->view.expand_row(path, true);
  }
}

/* ---------------------------------------------------------------- */

void
ItemBrowserBase::on_view_button_pressed (GdkEventButton* event)
{
  if (event->type != GDK_BUTTON_PRESS || event->button != 3)
    return;

  Glib::RefPtr<Gtk::TreeView::Selection> selection
      = this->view.get_selection();

  if (selection->count_selected_rows() != 1)
    return;

  Gtk::TreeModel::iterator iter = selection->get_selected();
  ApiElement const* elem = (*iter)[this->cols.data];
  if (elem == 0)
    return;

  /* Only skills have context menus for now. */
  switch (elem->get_type())
  {
    case API_ELEM_SKILL:
    {
      ApiSkill const* skill = (ApiSkill const*)elem;

      GtkSkillContextMenu* menu = Gtk::manage(new GtkSkillContextMenu);
      menu->set_skill(skill, this->charsheet->get_level_for_skill(skill->id));
      menu->popup(event->button, event->time);
      menu->signal_planning_requested().connect(sigc::mem_fun
          (*this, &ItemBrowserBase::on_planning_requested));
    }
    default:
      break;
  }

  return;
}

/* ---------------------------------------------------------------- */

bool
ItemBrowserBase::on_query_element_tooltip (int x, int y, bool key,
    Glib::RefPtr<Gtk::Tooltip> const& tooltip)
{
  key = false;

  Gtk::TreeModel::Path path;
  Gtk::TreeViewDropPosition pos;

  bool exists = this->view.get_dest_row_at_pos(x, y, path, pos);

  if (!exists)
    return false;

  Gtk::TreeIter iter = this->store->get_iter(path);
  ApiElement const* elem = (*iter)[this->cols.data];

  if (elem == 0)
    return false;

  switch (elem->get_type())
  {
    case API_ELEM_SKILL:
      GtkHelpers::create_tooltip(tooltip, (ApiSkill const*)elem);
      break;
    case API_ELEM_CERT:
      GtkHelpers::create_tooltip(tooltip, (ApiCert const*)elem);
      break;
    default:
      return false;
  }

  return true;
}

/* ================================================================ */

enum ComboBoxSkillFilter
{
  CB_FILTER_SKILL_ALL,
  CB_FILTER_SKILL_UNKNOWN,
  CB_FILTER_SKILL_PARTIAL,
  CB_FILTER_SKILL_ENABLED,
  CB_FILTER_SKILL_KNOWN
};

GtkSkillBrowser::GtkSkillBrowser (void)
  : Gtk::VBox(false, 5)
{
  this->store->set_sort_column(this->cols.name, Gtk::SORT_ASCENDING);

  this->filter_cb.append_text("Show all skills");
  this->filter_cb.append_text("Only show unknown skills");
  this->filter_cb.append_text("Only show partial skills");
  this->filter_cb.append_text("Only show enabled skills");
  this->filter_cb.append_text("Only show known skills");
  this->filter_cb.set_active(0);

  Gtk::ScrolledWindow* scwin = MK_SCWIN;
  scwin->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  scwin->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
  scwin->add(this->view);

  Gtk::Button* clear_filter_but = MK_BUT0;
  clear_filter_but->set_image(*MK_IMG(Gtk::Stock::CLEAR, Gtk::ICON_SIZE_MENU));
  clear_filter_but->set_relief(Gtk::RELIEF_NONE);
  clear_filter_but->set_tooltip_text("Clear filter");

  Gtk::HBox* filter_box = MK_HBOX;
  filter_box->pack_start(*MK_LABEL("Filter:"), false, false, 0);
  filter_box->pack_start(this->filter_entry, true, true, 0);
  filter_box->pack_start(*clear_filter_but, false, false, 0);

  this->pack_start(*filter_box, false, false, 0);
  this->pack_start(this->filter_cb, false, false, 0);
  this->pack_start(*scwin, true, true, 0);

  this->filter_entry.signal_activate().connect(sigc::mem_fun
      (*this, &GtkSkillBrowser::fill_store));
  this->filter_cb.signal_changed().connect(sigc::mem_fun
      (*this, &GtkSkillBrowser::fill_store));
  clear_filter_but->signal_clicked().connect(sigc::mem_fun
      (*this, &GtkSkillBrowser::clear_filter));
}

/* ---------------------------------------------------------------- */

void
GtkSkillBrowser::fill_store (void)
{
  this->store->clear();
  Glib::ustring filter = this->filter_entry.get_text();

  ApiSkillTreePtr tree = ApiSkillTree::request();
  ApiSkillMap& skills = tree->skills;
  ApiSkillGroupMap& groups = tree->groups;

  typedef Gtk::TreeModel::iterator GtkTreeModelIter;
  typedef std::map<int, std::pair<GtkTreeModelIter, int> > SkillGroupsMap;
  SkillGroupsMap skill_group_iters;

  /* Append all skill groups to the store. */
  for (ApiSkillGroupMap::iterator iter = groups.begin();
      iter != groups.end(); iter++)
  {
    Gtk::TreeModel::iterator siter = this->store->append();
    (*siter)[this->cols.name] = iter->second.name;
    (*siter)[this->cols.icon] = ImageStore::skillicons[0];
    (*siter)[this->cols.data] = 0;
    skill_group_iters.insert(std::make_pair
        (iter->first, std::make_pair(siter, 0)));
  }

  /* Prepare some short hands .*/
  int active_row_num = this->filter_cb.get_active_row_number();
  bool only_unknown = (active_row_num == CB_FILTER_SKILL_UNKNOWN);
  bool only_partial = (active_row_num == CB_FILTER_SKILL_PARTIAL);
  bool only_enabled = (active_row_num == CB_FILTER_SKILL_ENABLED);
  bool only_known = (active_row_num == CB_FILTER_SKILL_KNOWN);

  /* Append all skills to the skill groups. */
  for (ApiSkillMap::iterator iter = skills.begin();
      iter != skills.end(); iter++)
  {
    ApiSkill& skill = iter->second;

    /* Apply string filter. */
    if (Glib::ustring(skill.name).casefold()
        .find(filter.casefold()) == Glib::ustring::npos)
      continue;

    SkillGroupsMap::iterator giter = skill_group_iters.find(skill.group);
    if (giter == skill_group_iters.end())
    {
      std::cout << "Error appending skill, unknown group!" << std::endl;
      continue;
    }

    ApiCharSheetSkill* cskill = this->charsheet->get_skill_for_id(skill.id);
    Glib::RefPtr<Gdk::Pixbuf> skill_icon;

    if (cskill == 0)
    {
      /* The skill is unknown. */
      if (only_known || only_partial)
        continue;

      if (this->have_prerequisites_for_skill(&skill))
      {
        /* The skill is unknown but prequisites are there. */
        skill_icon = ImageStore::skillstatus[1];
      }
      else
      {
        /* The skill is unknown and no prequisites. */
        if (only_enabled)
          continue;
        skill_icon = ImageStore::skillstatus[0];
      }

    }
    else
    {
      /* The skill is known. */
      if (only_unknown || only_enabled)
        continue;

      /* Check if the skill is partially trained. */
      if (only_partial && cskill->points == cskill->points_start)
        continue;

      switch (cskill->level)
      {
        case 0: skill_icon = ImageStore::skillstatus[2]; break;
        case 1: skill_icon = ImageStore::skillstatus[3]; break;
        case 2: skill_icon = ImageStore::skillstatus[4]; break;
        case 3: skill_icon = ImageStore::skillstatus[5]; break;
        case 4: skill_icon = ImageStore::skillstatus[6]; break;
        case 5: skill_icon = ImageStore::skillstatus[7]; break;
        default: skill_icon = ImageStore::skillstatus[0]; break;
      }
    }

    /* Finally append the skill. */
    Gtk::TreeModel::iterator siter = this->store->append
        (giter->second.first->children());
    (*siter)[this->cols.name] = skill.name + " ("
        + Helpers::get_string_from_int(skill.rank) + ")";
    (*siter)[this->cols.data] = &skill;

    (*siter)[this->cols.icon] = skill_icon;

    giter->second.second += 1;
  }

  /* Remove empty groups (due to filtering). */
  for (SkillGroupsMap::iterator iter = skill_group_iters.begin();
      iter != skill_group_iters.end(); iter++)
  {
    if (iter->second.second == 0)
      this->store->erase(iter->second.first);
  }

  if (!filter.empty() || active_row_num != 0)
    this->view.expand_all();
}

/* ---------------------------------------------------------------- */

void
GtkSkillBrowser::clear_filter (void)
{
  this->filter_entry.set_text("");
  this->fill_store();
}

/* ---------------------------------------------------------------- */

bool
GtkSkillBrowser::have_prerequisites_for_skill (ApiSkill const* skill)
{
  ApiSkillTreePtr tree = ApiSkillTree::request();
  for (unsigned int i = 0; i < skill->deps.size(); ++i)
  {
    int depskill_id = skill->deps[i].first;
    int depskill_level = skill->deps[i].second;

    int charlevel = this->charsheet->get_level_for_skill(depskill_id);
    if (charlevel < depskill_level)
      return false;
  }

  return true;
}

/* ================================================================ */

enum ComboBoxCertFilter
{
  CB_FILTER_CERT_ALL,
  CB_FILTER_CERT_CLAIMED,
  CB_FILTER_CERT_CLAIMABLE,
  CB_FILTER_CERT_PARTIAL,
  CB_FILTER_CERT_NOPRE
};

GtkCertBrowser::GtkCertBrowser (void)
  : Gtk::VBox(false, 5)
{
  this->filter_cb.append_text("Show all certificates");
  this->filter_cb.append_text("Only show claimed certs");
  this->filter_cb.append_text("Only show claimable certs");
  this->filter_cb.append_text("Only show partial certs");
  this->filter_cb.append_text("Only show unknown certs");
  this->filter_cb.set_active(0);

  Gtk::ScrolledWindow* scwin = MK_SCWIN;
  scwin->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  scwin->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
  scwin->add(this->view);

  Gtk::Button* clear_filter_but = MK_BUT0;
  clear_filter_but->set_image(*MK_IMG(Gtk::Stock::CLEAR, Gtk::ICON_SIZE_MENU));
  clear_filter_but->set_relief(Gtk::RELIEF_NONE);
  clear_filter_but->set_tooltip_text("Clear filter");

  Gtk::HBox* filter_box = MK_HBOX;
  filter_box->pack_start(*MK_LABEL("Filter:"), false, false, 0);
  filter_box->pack_start(this->filter_entry, true, true, 0);
  filter_box->pack_start(*clear_filter_but, false, false, 0);

  this->pack_start(*filter_box, false, false, 0);
  this->pack_start(this->filter_cb, false, false, 0);
  this->pack_start(*scwin, true, true, 0);

  this->filter_entry.signal_activate().connect(sigc::mem_fun
      (*this, &GtkCertBrowser::fill_store));
  this->filter_cb.signal_changed().connect(sigc::mem_fun
      (*this, &GtkCertBrowser::fill_store));
  clear_filter_but->signal_clicked().connect(sigc::mem_fun
      (*this, &GtkCertBrowser::clear_filter));
}

/* ---------------------------------------------------------------- */

void
GtkCertBrowser::fill_store (void)
{
  this->store->clear();
  Glib::ustring filter = this->filter_entry.get_text();

  ApiCertTreePtr tree = ApiCertTree::request();
  ApiCertMap& certs = tree->certificates;

  /* Prepare some short hands .*/
  int active_row_num = this->filter_cb.get_active_row_number();
  bool only_claimed = (active_row_num == CB_FILTER_CERT_CLAIMED);
  bool only_claimable = (active_row_num == CB_FILTER_CERT_CLAIMABLE);
  bool only_partial = (active_row_num == CB_FILTER_CERT_PARTIAL);
  bool only_unknown = (active_row_num == CB_FILTER_CERT_NOPRE);

  typedef std::pair<int, ApiCert const*> CertShowInfo;
  typedef std::map<Glib::ustring, CertShowInfo> CertClassMap;
  typedef std::map<int, CertClassMap> CertGradeMap;
  typedef std::map<Glib::ustring, CertGradeMap> CertCatMap;
  CertCatMap show_mapping;

  /* Iterate over all certificates, check character status and add to maps. */
  for (ApiCertMap::iterator iter = certs.begin(); iter != certs.end(); iter++)
  {
    ApiCert const* cert = &iter->second;
    ApiCertClass const* cclass = cert->class_details;
    ApiCertCategory const* cat = cclass->cat_details;

    /* Check if cert matches the filter. */
    if (!filter.empty() && Glib::ustring(cclass->name)
        .casefold().find(filter.casefold()) == Glib::ustring::npos)
      continue;

    CertShowInfo csi;
    csi.second = cert;
    int cgrade = this->charsheet->get_grade_for_class(cclass->id);
    if (cgrade < cert->grade)
    {
      if (only_claimed)
        continue;

      switch (this->check_prerequisites_for_cert(cert))
      {
        default:
        case CERT_PRE_HAVE_NONE:
          if (only_claimable || only_partial)
            continue;
          csi.first = 3;
          break;

        case CERT_PRE_HAVE_SOME:
          if (only_claimable || only_unknown)
            continue;
          csi.first = 2;
          break;

        case CERT_PRE_HAVE_ALL:
          if (only_partial || only_unknown)
            continue;
          csi.first = 1;
          break;
      }
    }
    else
    {
      if (only_claimable || only_partial || only_unknown)
        continue;

      csi.first = 0;
    }

    /* Certificate matched filters. Add to sets. */
    show_mapping[cat->name][cert->grade][cclass->name] = csi;
  }

  /* Fill the certificate store. */
  for (CertCatMap::iterator i = show_mapping.begin();
      i != show_mapping.end(); i++)
  {
    Gtk::TreeModel::iterator siter = this->store->append();
    (*siter)[this->cols.name] = i->first;
    (*siter)[this->cols.icon] = ImageStore::certificate_small;
    (*siter)[this->cols.data] = 0;

    for (CertGradeMap::iterator j = i->second.begin();
        j != i->second.end(); j++)
    {
      Gtk::TreeModel::iterator siter2 = this->store->append(siter->children());
      int grade_idx = ApiCertTree::get_grade_index(j->first);
      (*siter2)[this->cols.name] = ApiCertTree::get_name_for_grade(j->first);
      (*siter2)[this->cols.icon] = ImageStore::certgrades[grade_idx];
      (*siter2)[this->cols.data] = 0;

      for (CertClassMap::iterator k = j->second.begin();
          k != j->second.end(); k++)
      {
        Gtk::TreeModel::iterator row = this->store->append(siter2->children());
        (*row)[this->cols.name] = k->second.second->class_details->name;
        (*row)[this->cols.icon] = ImageStore::certstatus[k->second.first];
        (*row)[this->cols.data] = k->second.second;
      }
    }
  }

  if (!filter.empty() || active_row_num != 0)
    this->view.expand_all();
}

/* ---------------------------------------------------------------- */

void
GtkCertBrowser::clear_filter (void)
{
  this->filter_entry.set_text("");
  this->fill_store();
}

/* ---------------------------------------------------------------- */

GtkCertBrowser::CertPrerequisite
GtkCertBrowser::check_prerequisites_for_cert (ApiCert const* cert)
{
  unsigned int deps_amount = 0;
  unsigned int have_amount = 0;

  for (unsigned int i = 0; i < cert->skilldeps.size(); ++i)
  {
    int skill_id = cert->skilldeps[i].first;
    int skill_level = cert->skilldeps[i].second;

    if (this->charsheet->get_level_for_skill(skill_id) >= skill_level)
      have_amount += 1;
    deps_amount += 1;
  }

  ApiCertTreePtr tree = ApiCertTree::request();
  for (unsigned int i = 0; i < cert->certdeps.size(); ++i)
  {
    int cert_id = cert->certdeps[i].first;
    ApiCert const* rcert = tree->get_certificate_for_id(cert_id);
    int rcert_class_id = rcert->class_details->id;

    if (this->charsheet->get_grade_for_class(rcert_class_id) >= rcert->grade)
      have_amount += 1;
    deps_amount += 1;
  }

  if (have_amount == deps_amount)
    return CERT_PRE_HAVE_ALL;
  else if (have_amount == 0)
    return CERT_PRE_HAVE_NONE;
  else
    return CERT_PRE_HAVE_SOME;
}
