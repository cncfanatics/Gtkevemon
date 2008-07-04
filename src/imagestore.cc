#include "images/skill.h"
#include "images/eveportrait.h"
#include "images/applogo.h"
#include "images/aboutlogo.h"
#include "images/skillicons.h"
#include "images/skill_progress.h"

#include "imagestore.h"

Glib::RefPtr<Gdk::Pixbuf> ImageStore::skill;
Glib::RefPtr<Gdk::Pixbuf> ImageStore::applogo;
Glib::RefPtr<Gdk::Pixbuf> ImageStore::aboutlogo;
Glib::RefPtr<Gdk::Pixbuf> ImageStore::eveportrait;
Glib::RefPtr<Gdk::Pixbuf> ImageStore::skillicons[5];

/* ---------------------------------------------------------------- */

void
ImageStore::init (void)
{
  ImageStore::skill = Gdk::Pixbuf::create_from_xpm_data(skill_xpm);
  ImageStore::applogo = Gdk::Pixbuf::create_from_xpm_data(applogo_xpm);
  ImageStore::aboutlogo = Gdk::Pixbuf::create_from_xpm_data(aboutlogo_xpm);
  ImageStore::eveportrait = Gdk::Pixbuf::create_from_xpm_data(eveportrait_xpm);
  ImageStore::skillicons[0] = Gdk::Pixbuf::create_from_xpm_data
      (skillicons_group_xpm)->scale_simple(20, 20, Gdk::INTERP_BILINEAR);
  ImageStore::skillicons[1] = Gdk::Pixbuf::create_from_xpm_data
      (skillicons_skill_xpm)->scale_simple(20, 20, Gdk::INTERP_BILINEAR);
  ImageStore::skillicons[2] = Gdk::Pixbuf::create_from_xpm_data
      (skillicons_train_xpm)->scale_simple(20, 20, Gdk::INTERP_BILINEAR);
  ImageStore::skillicons[3] = Gdk::Pixbuf::create_from_xpm_data
      (skillicons_done_xpm)->scale_simple(20, 20, Gdk::INTERP_BILINEAR);
  ImageStore::skillicons[4] = Gdk::Pixbuf::create_from_xpm_data
      (skillicons_part_xpm)->scale_simple(20, 20, Gdk::INTERP_BILINEAR);
}

/* ---------------------------------------------------------------- */

Glib::RefPtr<Gdk::Pixbuf>
ImageStore::skill_progress (unsigned int level, double completed)
{
  Glib::RefPtr<Gdk::Pixbuf> ret(Gdk::Pixbuf::create_from_xpm_data
      (skill_progress_xpm));

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
