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
