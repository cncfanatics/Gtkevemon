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
    this->data = this->fetcher.request();
  }
  catch (Exception& e)
  {
    this->exception = e;
    this->data.reset();
  }

  this->sig_dispatch.emit();
  return 0;
}

/* ---------------------------------------------------------------- */

AsyncHttp::~AsyncHttp (void)
{
}
