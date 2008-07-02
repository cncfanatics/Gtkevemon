#ifndef API_IN_TRAINING_HEADER
#define API_IN_TRAINING_HEADER

#include <string>
#include <libxml/parser.h>

#include "ref_ptr.h"
#include "http.h"
#include "eveapi.h"
#include "apibase.h"

class ApiInTraining;
typedef ref_ptr<ApiInTraining> ApiInTrainingPtr;

class ApiInTraining : public ApiBase
{
  private:
    EveApiAuth auth;

  public:
    std::string start_time;
    std::string end_time;
    time_t start_time_t;
    time_t end_time_t;

    int skill;
    int start_sp;
    int dest_sp;
    int to_level;

    bool in_training;

  protected:
    ApiInTraining (EveApiAuth const& auth);
    void parse_xml (HttpDataPtr doc);
    void parse_recursive (xmlNodePtr node);

  public:
    static ApiInTrainingPtr create (EveApiAuth const& auth);
    void refresh (void);
};

#endif /* API_TRAINING_HEADER */
