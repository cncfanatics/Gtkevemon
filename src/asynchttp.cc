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
    this->http_result.data = this->request();
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
