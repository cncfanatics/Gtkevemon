/*
 * exception.h -- Part of the SimFM filemanager
 * Copyright (c) 2004 by Simon Fuhrmann
 *
 * Refer to `COPYING´ for license information.
 *
 * Description:
 * A simple class for storing and printing an exception string.
 */
#ifndef EXCEPTION_HEADER
#define EXCEPTION_HEADER

#include <iostream>
#include <string>

/* Universal simple exception class. */
class Exception : public std::string
{
  public:
    Exception (void) { }
    Exception (const std::string& msg) : std::string(msg) { }
    virtual ~Exception (void) { }

    void print (void);
};

/* ---------------------------------------------------------------- */

inline void
Exception::print (void)
{
  std::cout << "Exception: " << *this << std::endl;
}

#endif /* EXCEPTION_HEADER */
