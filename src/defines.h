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

#ifndef DEFINES_HEADER
#define DEFINES_HEADER

/* The version string used in the application. */
#define GTKEVEMON_VERSION_STR "Revision 1.7-101"

/* General compile-time configuration. */
#define LAUNCHER_CMD_AMOUNT 5

#ifndef WIN32
# define CONF_HOME_DIR ".gtkevemon"
#else
# define CONF_HOME_DIR "GtkEveMon"
#endif

#endif /* DEFINES_HEADER */
