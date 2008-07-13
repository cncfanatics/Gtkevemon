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

#ifndef IMAGE_STORE_HEADER
#define IMAGE_STORE_HEADER

#include <gdkmm/pixbuf.h>

class ImageStore
{
  public:
    static Glib::RefPtr<Gdk::Pixbuf> skill;
    static Glib::RefPtr<Gdk::Pixbuf> applogo;
    static Glib::RefPtr<Gdk::Pixbuf> aboutlogo;
    static Glib::RefPtr<Gdk::Pixbuf> eveportrait;
    static Glib::RefPtr<Gdk::Pixbuf> skillicons[5];

    static Glib::RefPtr<Gdk::Pixbuf> skill_progress
        (unsigned int level, double percent);

    static void init (void);
    static void unload (void);
};

#endif /* IMAGE_STORE_HEADER */
