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

#ifndef API_SKILL_QUEUE_HEADER
#define API_SKILL_QUEUE_HEADER

#include <vector>
#include <libxml/parser.h>

#include "ref_ptr.h"
#include "eveapi.h"
#include "apibase.h"

/* The minimum amount of seconds the sheet is cached. */
#define API_SKILL_QUEUE_MIN_CACHE_TIME 600

struct ApiSkillQueueItem
{
  int queue_pos;
  int skill_id;
  int to_level;
  int start_sp;
  int end_sp;
  std::string start_time;
  std::string end_time;
  time_t start_time_t;
  time_t end_time_t;
};

/* ---------------------------------------------------------------- */

class ApiSkillQueue;
typedef ref_ptr<ApiSkillQueue> ApiSkillQueuePtr;
typedef std::vector<ApiSkillQueueItem> ApiSkillQueueList;

class ApiSkillQueue : public ApiBase
{
  public:
    bool valid;
    std::vector<ApiSkillQueueItem> queue;

  protected:
    ApiSkillQueue (void);

    void parse_xml (void);
    void parse_eveapi_tag (xmlNodePtr node);
    void parse_result_tag (xmlNodePtr node);
    void parse_queue_rowset (xmlNodePtr node);

  public:
    static ApiSkillQueuePtr create (void);
    void set_api_data (EveApiData const& data);
    void debug_dump (void);

    /* Returns true if a skill is currently in training. */
    bool in_training (void) const;

    /* Returns the queue item currently in training. */
    ApiSkillQueueItem const* get_training_skill (void) const;

    /* Returns the SP/h of the skill currently in training,
     * or 0 if there is no skill in training. */
    unsigned int get_spph_for_current (void) const;
};

/* ---------------------------------------------------------------- */

inline
ApiSkillQueue::ApiSkillQueue (void)
{
}

inline ApiSkillQueuePtr
ApiSkillQueue::create (void)
{
  return ApiSkillQueuePtr(new ApiSkillQueue);
}

#endif /* API_SKILL_QUEUE_HEADER */
