#ifndef GTK_PORTRAIT_HEADER
#define GTK_PORTRAIT_HEADER

#include <gtkmm/image.h>

/* The size of the portrait (pixels) in the GUI. */
#define PORTRAIT_SIZE 85

class GtkPortrait : public Gtk::Image
{
  private:
    static sigc::signal<void> sig_update_portraits;

  public:
    static void emit_update_signal (void);

  private:
    std::string char_id;
    sigc::connection update_con;

    bool fetch_from_gtkevemon_cache (void);
    bool fetch_from_eve_cache (void);
    bool fetch_from_eve_online (void);
    void cache_portrait (Glib::RefPtr<Gdk::Pixbuf> portrait);

  public:
    GtkPortrait (void);
    GtkPortrait (std::string const& charid);
    ~GtkPortrait (void);

    void set (std::string const& charid);
    void update (void);
};

#endif /* GTK_PORTRAIT_HEADER */
