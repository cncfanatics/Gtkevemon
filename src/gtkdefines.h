#ifndef GTK_DEFINES_HEADER
#define GTK_DEFINES_HEADER

#define MK_VBOX Gtk::manage(new Gtk::VBox(false, 5))
#define MK_HBOX Gtk::manage(new Gtk::HBox(false, 5))
#define MK_VBOX0 Gtk::manage(new Gtk::VBox(false, 0))
#define MK_HBOX0 Gtk::manage(new Gtk::HBox(false, 0))
#define MK_FRAME(str) Gtk::manage(new Gtk::Frame(str))
#define MK_FRAME0 Gtk::manage(new Gtk::Frame)
#define MK_LABEL(str) Gtk::manage(new Gtk::Label(str))
#define MK_LABEL0 Gtk::manage(new Gtk::Label)
#define MK_HSEP Gtk::manage(new Gtk::HSeparator)
#define MK_VSEP Gtk::manage(new Gtk::VSeparator)
#define MK_BUT(ctor) Gtk::manage(new Gtk::Button(ctor))
#define MK_SCWIN Gtk::manage(new Gtk::ScrolledWindow)

#endif /* GTK_DEFINES_HEADER */
