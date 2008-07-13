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

#ifndef THREAD_HEADER
#define THREAD_HEADER

#include <pthread.h>
#include <semaphore.h>

class Thread
{
  private:
    pthread_t handle;

    static void* stub (void* arg)
    { return ((Thread*)arg)->run(); }

  protected:
    virtual void* run (void) = 0;

  public:
    virtual ~Thread (void) { }

    /* Creates a new thread and runs the run() method. */
    void pt_create (const pthread_attr_t* p = 0)
    { pthread_create(&handle, p, Thread::stub, (void*)this); }

    /* Sends a cancelation request to the thread. */
    void pt_cancel (void)
    { pthread_cancel(handle); }

    /* Blocks and waits for termination of the thread. */
    void pt_join (void)
    { pthread_join(handle, 0); }
};

/* ---------------------------------------------------------------- */

class Semaphore
{
  private:
    sem_t sem;

  public:
    /* Defaults to mutex. */
    Semaphore (unsigned int value = 1)
    { sem_init(&sem, 0, value); }

    Semaphore (unsigned int value, int pshared)
    { sem_init(&sem, pshared, value); }

    /* DOWN the semaphore. */
    int wait (void)
    { return sem_wait(&sem); }

    /* UP the semaphore. */
    int post (void)
    { return sem_post(&sem); }

    /* Returns the current value. */
    int get_value (void)
    {
      int value;
      sem_getvalue(&sem, &value);
      return value;
    }
};

#endif /* THREAD_HEADER */
