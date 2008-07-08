#ifndef ASYNC_HTTP_HEADER
#define ASYNC_HTTP_HEADER

#include <glibmm/dispatcher.h>

#include "http.h"
#include "thread.h"
#include "exception.h"

/* This is delivered when the request ist done.
 * The data member is NULL if there was an error.
 * The exception member is the error description then.
 */
class AsyncHttpData
{
  public:
    HttpDataPtr data;
    Exception exception;
};

/* Class for asynchronous HTTP requests. Instructions:
 * - Create class with create()
 * - Setup the HTTP data (host, path, etc)
 * - Connect to the done signal (disconnect if not interested anymore)
 * - Run async_request()
 * - Data will be delivered to all signal subscribers
 * - No need to free, automatic deletion if all signals are processed
 */
class AsyncHttp : public Thread, public Http
{
  private:
    AsyncHttpData http_result;
    Glib::Dispatcher sig_dispatch;
    sigc::signal<void, AsyncHttpData> sig_done;

  protected:
    AsyncHttp (void);

    void* run (void);
    void dispatch (void);

  public:
    static AsyncHttp* create (void);
    ~AsyncHttp (void);

    void async_request (void);
    sigc::signal<void, AsyncHttpData>& signal_done (void);
};

/* ---------------------------------------------------------------- */

inline AsyncHttp*
AsyncHttp::create (void)
{
  return new AsyncHttp;
}

inline void
AsyncHttp::async_request (void)
{
  this->pt_create();
}

inline void
AsyncHttp::dispatch (void)
{
  this->sig_done.emit(this->http_result);
  delete this;
}

inline sigc::signal<void, AsyncHttpData>&
AsyncHttp::signal_done (void)
{
  return sig_done;
}

#endif /* ASYNC_HTTP_HEADER */
