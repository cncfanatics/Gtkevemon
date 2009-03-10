#include <sstream>

#include "httpstatus.h"
#include "asynchttp.h"

AsyncHttp::AsyncHttp (void)
{
  this->sig_dispatch.connect(sigc::mem_fun(*this, &AsyncHttp::dispatch));
}

/* ---------------------------------------------------------------- */

void*
AsyncHttp::run (void)
{
  try
  {
    HttpDataPtr data = this->request();
    this->http_result.data = data;

    /* If we receive a HTTP status code other than 200,
     * the exception text is set for easy status reporting. */
    if (data.get() != 0 && data->http_code != 200)
    {
      std::stringstream ss;
      ss << "Received HTTP code " << data->http_code
          << ": " << HttpStatus::get_string(data->http_code);
      this->http_result.exception = ss.str();
    }
  }
  catch (Exception& e)
  {
    this->http_result.exception = e;
    this->http_result.data.reset();
  }

  this->sig_dispatch.emit();
  return 0;
}

/* ---------------------------------------------------------------- */

AsyncHttp::~AsyncHttp (void)
{
}
