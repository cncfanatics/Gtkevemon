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

#ifndef API_IN_TRAINING_HEADER
#define API_IN_TRAINING_HEADER

#include <string>
#include <libxml/parser.h>

#include "ref_ptr.h"
#include "http.h"
#include "apibase.h"

class ApiInTraining;
typedef ref_ptr<ApiInTraining> ApiInTrainingPtr;

class ApiInTraining : public ApiBase
{
  /* Some internal stuff. */
  protected:
    ApiInTraining (void);
    void parse_xml (HttpDataPtr doc);
    void parse_recursive (xmlNodePtr node);

  /* Publicly available collection of gathered data. */
  public:
    bool valid;
    bool in_training;

    std::string start_time;
    std::string end_time;
    time_t start_time_t;
    time_t end_time_t;

    int skill;
    int start_sp;
    int dest_sp;
    int to_level;

  public:
    static ApiInTrainingPtr create (void);
    void set_from_xml (HttpDataPtr xmldata);
};

/* ---------------------------------------------------------------- */

inline
ApiInTraining::ApiInTraining (void) : valid(false)
{
}

/* ---------------------------------------------------------------- */

inline ApiInTrainingPtr
ApiInTraining::create (void)
{
  ApiInTrainingPtr obj(new ApiInTraining);
  return obj;
}

#endif /* API_TRAINING_HEADER */
