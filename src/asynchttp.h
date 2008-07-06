#ifndef ASYNC_HTTP_HEADER
#define ASYNC_HTTP_HEADER

#include <glibmm/dispatcher.h>

#include "http.h"
#include "thread.h"
#include "exception.h"

/* Class for asynchronous HTTP requests. Instructions:
 * - Create class on the heap (with new)
 * - Setup the HTTP fetcher member
 * - Connect to the done signal (disconnect if not interested anymore)
 * - Run async_request
 *   - data will be NULL and exception is filled on error
 */
class AsyncHttp : public Thread
{
  private:
    Glib::Dispatcher sig_dispatch;
    sigc::signal<void, AsyncHttp*> sig_done;

  protected:
    void* run (void);
    void dispatch (void);

  public:
    Http fetcher;
    HttpDataPtr data;
    Exception exception;

  public:
    AsyncHttp (void);
    ~AsyncHttp (void);
    void async_request (void);
    sigc::signal<void, AsyncHttp*>& signal_done (void);
};

/* ---------------------------------------------------------------- */

inline void
AsyncHttp::async_request (void)
{
  this->create();
}

inline void
AsyncHttp::dispatch (void)
{
  this->sig_done.emit(this);
  delete this;
}

inline sigc::signal<void, AsyncHttp*>&
AsyncHttp::signal_done (void)
{
  return sig_done;
}

#endif /* ASYNC_HTTP_HEADER */
