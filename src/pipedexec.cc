#ifdef WIN32
#  include "pipedexec_win32.cc"
#else
#  include "pipedexec_posix.cc"
#endif
