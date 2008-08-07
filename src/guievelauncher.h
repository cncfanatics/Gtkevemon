#ifndef GUI_EVE_LAUNCHER
#define GUI_EVE_LAUNCHER

#include "winbase.h"

class GuiEveLauncher : public WinBase
{
  private:
    void run_command (std::string const& cmd);

  protected:
    GuiEveLauncher (std::vector<std::string> const& cmds);

  public:
    static Gtk::Window* launch_eve (void);
};

#endif /* GUI_EVE_LAUNCHER */
