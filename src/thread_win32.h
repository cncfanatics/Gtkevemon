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

#ifndef WIN32_THREAD_HEADER
#define WIN32_THREAD_HEADER

#include <iostream>
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>

class Thread
{
  private:
    HANDLE handle;
    HANDLE cancel_event;
    bool cleanup;

    static void* stub (void* arg)
    {
      return ((Thread*)arg)->run();
    }

  protected:
    virtual void* run (void) = 0;

  public:
    Thread (void)
    { this->cleanup = false; }

    virtual ~Thread (void)
    {
      if (this->cleanup)
      {
        CloseHandle(this->cancel_event);
        CloseHandle(this->handle);
      }
    }

    /* Creates a new thread and runs the run() method. */
    // FIXME: Abstract pthread_attr_t
    void pt_create (void)
    {
      this->cleanup = true;
      this->cancel_event = CreateEvent(NULL, FALSE, FALSE, NULL);
      this->handle = CreateThread(NULL, 0,
          (LPTHREAD_START_ROUTINE)Thread::stub, (void*)this, 0, NULL);

      if (this->handle == NULL)
        std::cerr << "ERROR: CreateThread(): " << GetLastError() << std::endl;
    }

    /* Sends a cancelation request to the thread. */
    void pt_cancel (void)
    {
      SetEvent(this->cancel_event);
    }

    /* Blocks and waits for termination of the thread. */
    void pt_join (void)
    {
      this->cleanup = false;
      WaitForSingleObject(this->handle, INFINITE);
      CloseHandle(this->cancel_event);
      CloseHandle(this->handle);
    }
};

/* ---------------------------------------------------------------- */

class Semaphore
{
  private:
    HANDLE sem;
    volatile LONG value;

  public:
    /* Defaults to mutex. */
    Semaphore (unsigned int value = 1)
    {
      this->value = value;
      this->sem = CreateSemaphore(NULL, value, value, NULL);
    }

    /* DOWN the semaphore. */
    int wait (void)
    {
      int retval = WaitForSingleObject(this->sem, INFINITE);
      if (retval == WAIT_OBJECT_0)
      {
        InterlockedDecrement(&this->value);
        return 0;
      }
      return -1;
    }

    /* UP the semaphore. */
    int post (void)
    {
      InterlockedIncrement(&this->value);
      if (ReleaseSemaphore(this->sem, 1, NULL))
      {
        InterlockedDecrement(&this->value);
        return -1;
      }
      return 0;
    }

    /* Returns the current value. */
    int get_value (void)
    {
      return this->value;
    }
};

#endif /* WIN32_THREAD_HEADER */
