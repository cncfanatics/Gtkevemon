#include "images/skill.h"
#include "images/skillstatus.h"
#include "images/skillicons.h"
#include "images/skillprogress.h"
#include "images/skilldeps.h"
#include "images/skillplan.h"
#include "images/eveportrait.h"
#include "images/applogo.h"
#include "images/aboutlogo.h"
#include "images/menuicons.h"
#include "images/guiimages.h"

#include "imagestore.h"

Glib::RefPtr<Gdk::Pixbuf> ImageStore::skill;
Glib::RefPtr<Gdk::Pixbuf> ImageStore::applogo;
Glib::RefPtr<Gdk::Pixbuf> ImageStore::aboutlogo;
Glib::RefPtr<Gdk::Pixbuf> ImageStore::eveportrait;
Glib::RefPtr<Gdk::Pixbuf> ImageStore::columnconf;
Glib::RefPtr<Gdk::Pixbuf> ImageStore::skillicons[6];
Glib::RefPtr<Gdk::Pixbuf> ImageStore::skillstatus[8];
Glib::RefPtr<Gdk::Pixbuf> ImageStore::skilldeps[3];
Glib::RefPtr<Gdk::Pixbuf> ImageStore::skillplan[5];
Glib::RefPtr<Gdk::Pixbuf> ImageStore::menuicons[3];

/* ---------------------------------------------------------------- */

void
ImageStore::init (void)
{
  ImageStore::skill = Gdk::Pixbuf::create_from_inline
      (-1, img_skill, false);
  ImageStore::applogo = Gdk::Pixbuf::create_from_xpm_data
      (img_applogo_xpm);
  ImageStore::aboutlogo = Gdk::Pixbuf::create_from_xpm_data
      (img_aboutlogo_xpm);
  ImageStore::eveportrait = Gdk::Pixbuf::create_from_xpm_data
      (img_eveportrait_xpm);
  ImageStore::columnconf = Gdk::Pixbuf::create_from_inline
      (-1, img_columnconf_png, false);

  ImageStore::skillicons[0] = Gdk::Pixbuf::create_from_inline
      (-1, img_skillicons_group, false);
  ImageStore::skillicons[1] = Gdk::Pixbuf::create_from_inline
      (-1, img_skillicons_skill, false);
  ImageStore::skillicons[2] = Gdk::Pixbuf::create_from_inline
      (-1, img_skillicons_train, false);
  ImageStore::skillicons[3] = Gdk::Pixbuf::create_from_inline
      (-1, img_skillicons_done, false);
  ImageStore::skillicons[4] = Gdk::Pixbuf::create_from_inline
      (-1, img_skillicons_part, false);
  ImageStore::skillicons[5] = Gdk::Pixbuf::create_from_inline
      (-1, img_skillicons_faded, false);

  ImageStore::skillstatus[0] = Gdk::Pixbuf::create_from_xpm_data
      (img_skillstatus_nopre_xpm);
  ImageStore::skillstatus[1] = Gdk::Pixbuf::create_from_xpm_data
      (img_skillstatus_havepre_xpm);
  ImageStore::skillstatus[2] = Gdk::Pixbuf::create_from_xpm_data
      (img_skillstatus_at0_xpm);
  ImageStore::skillstatus[3] = Gdk::Pixbuf::create_from_xpm_data
      (img_skillstatus_at1_xpm);
  ImageStore::skillstatus[4] = Gdk::Pixbuf::create_from_xpm_data
      (img_skillstatus_at2_xpm);
  ImageStore::skillstatus[5] = Gdk::Pixbuf::create_from_xpm_data
      (img_skillstatus_at3_xpm);
  ImageStore::skillstatus[6] = Gdk::Pixbuf::create_from_xpm_data
      (img_skillstatus_at4_xpm);
  ImageStore::skillstatus[7] = Gdk::Pixbuf::create_from_xpm_data
      (img_skillstatus_at5_xpm);

  ImageStore::skilldeps[0] = Gdk::Pixbuf::create_from_inline
      (-1, img_skilldeps_none, false);
  ImageStore::skilldeps[1] = Gdk::Pixbuf::create_from_inline
      (-1, img_skilldeps_partial, false);
  ImageStore::skilldeps[2] = Gdk::Pixbuf::create_from_inline
      (-1, img_skilldeps_have, false);

  ImageStore::skillplan[0] = Gdk::Pixbuf::create_from_inline
      (-1, img_skillplan_trained, false);
  ImageStore::skillplan[1] = Gdk::Pixbuf::create_from_inline
      (-1, img_skillplan_training, false);
  ImageStore::skillplan[2] = Gdk::Pixbuf::create_from_inline
      (-1, img_skillplan_trainable, false);
  ImageStore::skillplan[3] = Gdk::Pixbuf::create_from_inline
      (-1, img_skillplan_untrainable, false);
  ImageStore::skillplan[4] = Gdk::Pixbuf::create_from_inline
      (-1, img_skillplan_deperror, false);

  ImageStore::menuicons[0] = Gdk::Pixbuf::create_from_xpm_data
      (img_menu_evemon_xpm);
  ImageStore::menuicons[1] = Gdk::Pixbuf::create_from_xpm_data
      (img_menu_char_xpm);
  ImageStore::menuicons[2] = Gdk::Pixbuf::create_from_xpm_data
      (img_menu_help_xpm);
}

/* ---------------------------------------------------------------- */

Glib::RefPtr<Gdk::Pixbuf>
ImageStore::skill_progress (unsigned int level, double completed)
{
  Glib::RefPtr<Gdk::Pixbuf> ret(Gdk::Pixbuf::create_from_xpm_data
      (img_skillprogress_xpm));

  /* Some safety checks. */
  if (level > 5) level = 5;
  if (completed < 0.0) completed = 0.0;
  if (completed > 1.0) completed = 1.0;

  /* draw level */
  for (unsigned int l = 0; l < level; ++l)
    for (unsigned int x = 0; x < 6; ++x)
      ret->copy_area(0, 0, 1, 5, ret, (l * 7 + x + 2), 2);

  /* draw percent */
  for (unsigned int p = 0; p < (unsigned int)::round(34.0 * completed); ++p)
    ret->copy_area(0, 0, 1, 2, ret, (p + 2), 13);

  return ret;
}

/* ---------------------------------------------------------------- */

void
ImageStore::unload (void)
{
}
