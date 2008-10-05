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

#ifndef MAILER_HEADER
#define MAILER_HEADER

#include <string>

class Mailer
{
  public:
    static int send (std::string const& address, std::string const& subject,
        std::string const& message);
};

#endif /* MAILER_HEADER */
