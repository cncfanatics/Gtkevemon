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

#include "eveapi.h"
#include "apibase.h"

// queuePosition="1" typeID="11441" level="3" startSP="7072" endSP="40000" startTime="2009-03-18 02:01:06" endTime="2009-03-18 15:19:21"
struct ApiSkillQueueItem
{
  int queue_pos;
  int skill_id;
  int skill_level;
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
    std::vector<ApiSkillQueueItem> items;

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
