/*
 *  A reference counting smart pointer for C++
 *  Copyright (c) 2005-2006 by Simon Fuhrmann
 *
 *  This code is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 dated June, 1991.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the application. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef REF_PTR_HEADER
#define REF_PTR_HEADER

template <class T>
class ref_ptr
{
  /* Private declaration of class members. */
  private:
    T* ptr;
    int* count;

  /* Private definition of member methods. */
  private:
    void increment (void)
      {
        if (ptr == 0) return;
        ++(*count);
      }

    void decrement (void)
      {
        if (ptr == 0) return;
        if (--(*count) == 0)
          {
            delete count;
            delete ptr;
          }
      }

  /* Public definition of member methods. */
  public:
    /* Ctor: Default one. */
    ref_ptr (void) : ptr(0), count(0)
      { }

    /* Ctor: From pointer. */
    explicit ref_ptr (T* p) : ptr(p)
      { count = (p == 0) ? 0 : new int(1); }

    /* Ctor: Copy from other ref_ptr. */
    ref_ptr (const ref_ptr<T>& src) : ptr(src.ptr), count(src.count)
      { increment(); }

    /* Destructor. */
    ~ref_ptr (void)
      { decrement(); }

    /* Assignment: From other ref_ptr. */
    ref_ptr<T>& operator= (const ref_ptr<T>& rhs)
      {
	if (rhs.ptr == ptr) return *this;
	decrement();
        ptr = rhs.ptr;
        count = rhs.count;
        increment();
        return *this;
      }

    /* Assignment: From pointer. */
    ref_ptr<T>& operator= (T* rhs)
      {
	if (rhs == ptr) return *this;
	decrement();
        ptr = rhs;
        count = (ptr == 0) ? 0 : new int(1);
	return *this;
      }

    /* Operations. */
    void reset (void)
      {
        decrement();
	ptr = 0; count = 0;
      }

    void swap (ref_ptr<T>& p)
      {
        T* tp = p.ptr; p.ptr = ptr; ptr = tp;
	int *tc = p.count; p.count = count; count = tc;
      }

    /* Dereference. */
    T& operator* (void) const
      { return *ptr; }

    T* operator-> (void) const
      { return ptr; }

    /* Comparison. */
    bool operator== (const T* p) const
      { return p == ptr; }

    bool operator== (const ref_ptr<T>& p) const
      { return p.ptr == ptr; }

    bool operator!= (const T* p) const
      { return p != ptr; }

    bool operator!= (const ref_ptr<T>& p) const
      { return p.ptr != ptr; }

    bool operator< (const ref_ptr<T>& rhs)
      { return ptr < rhs.ptr; }

    /* Information. */
    int use_count (void) const
      { return (count == 0) ? 0 : *count; }

    T* get (void) const
      { return ptr; }

  #ifndef NO_MEMBER_TEMPLATES

  /* Template friends for accessing diffrent ref_ptr's. */
  private:
    template <class Y> friend class ref_ptr;

  public:
    /* Ctor: From diffrent pointer. */
    template <class Y>
    explicit ref_ptr (Y* p) : ptr(static_cast<T*>(p))
      { count = (p == 0) ? 0 : new int(1); }

    /* Ctor: Copy from diffrent ref_ptr. */
    template <class Y>
    ref_ptr (const ref_ptr<Y>& src)
      {
        ptr = static_cast<T*>(src.ptr);
        count = src.count;
        increment();
      }

    /* Assignment: From diffrent ref_ptr. */
    template <class Y>
    ref_ptr<T>& operator= (const ref_ptr<Y>& rhs)
      {
        if (rhs.ptr == ptr) return *this;
        decrement();
	ptr = static_cast<T*>(rhs.ptr);
	count = rhs.count;
	increment();
	return *this;
      }

    /* Assignment: From diffrent pointer. */
    template <class Y>
    ref_ptr<T>& operator= (Y* rhs)
      {
        if (rhs == ptr) return *this;
	decrement();
	ptr = static_cast<T*>(rhs);
	count = (ptr == 0) ? 0 : new int(1);
	return *this;
      }

    /* Comparison with diffrent ref_ptr type. */
    template <class Y>
    bool operator== (const ref_ptr<Y>& p) const
      { return p.ptr == ptr; }

    template <class Y>
    bool operator!= (const ref_ptr<Y>& p) const
      { return p.ptr != ptr; }

    template <class Y>
    bool operator< (const ref_ptr<Y>& rhs) const
      { return ptr < rhs.ptr; }

  #endif /* NO_MEMBER_TEMPLATES */
};

#endif /* REF_PTR_HEADER */
