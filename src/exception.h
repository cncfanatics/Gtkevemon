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

#include <exception>
#include <string>

/* Universal simple exception class. */
class Exception : public std::exception, public std::string
{
  public:
    Exception (void) throw()
    { }

    Exception (std::string const& msg) throw() : std::string(msg)
    { }

    virtual ~Exception (void) throw()
    { }

    virtual const char* what (void) const throw()
    { return this->c_str(); }
};

/* ---------------------------------------------------------------- */

/* More detailed exception for file errors. */
class FileException : public Exception
{
  public:
    std::string name;

  public:
    FileException (std::string const& name, std::string const& msg) throw()
        : Exception(msg), name(name)
    { }

    virtual ~FileException (void) throw()
    { }
};

#endif /* EXCEPTION_HEADER */
