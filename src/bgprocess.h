#ifndef BG_PROCESS_HEADER
#define BG_PROCESS_HEADER

#include <vector>
#include <string>

#include "thread.h"

/*
 * This class executes a new progress and waits for
 * its termination in a thread. After termination the
 * thread will end.
 *
 * Always create this object on the heap (use "new BGProcess").
 * The object will free itself on completition.
 */
class BGProcess : public Thread
{
  private:
    std::vector<std::string> cmd;
    std::string chdir;

    void* run (void);
    char** create_argv (std::vector<std::string> const& cmd);

  public:
    BGProcess (std::vector<std::string>& cmd, std::string const& chdir = "");
};

#endif /* BG_PROCESS_HEADER */
