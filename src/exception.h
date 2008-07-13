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

#ifndef EXCEPTION_HEADER
#define EXCEPTION_HEADER

#include <string>

/* Universal simple exception class. */
class Exception : public std::string
{
  public:
    Exception (void) { }
    Exception (const std::string& msg) : std::string(msg) { }
    virtual ~Exception (void) { }
};

#endif /* EXCEPTION_HEADER */
