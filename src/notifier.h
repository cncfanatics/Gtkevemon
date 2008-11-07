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

#ifndef NOTIFIER_HEADER
#define NOTIFIER_HEADER

#include <string>

#include "apicharsheet.h"
#include "apiintraining.h"

/* This class handles the notification to external applications. */
class Notifier
{
  private:
    static void replace (std::string& subject, std::string const& find,
        std::string const& replace);

  public:
    static int exec (ApiCharSheetPtr sheet, ApiInTrainingPtr training);
};

#endif /* NOTIFIER_HEADER */
