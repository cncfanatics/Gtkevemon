/*
 * conf.h -- Part of the SimFM filemanager
 * Copyright (c) 2004 by Simon Fuhrmann
 *
 * Refer to `COPYING� for license information.
 *
 * Description:
 * This is a collection of classes to handle ini-styled config files.
 * Unlike regular ini-styled config files, it is possible to put values
 * _and_ sections to sections, so one can define a tree with sections as
 * inner nodes and values as leafs.
 */
#ifndef CONF_HEADER
#define CONF_HEADER

#include <ostream>
#include <string>
#include <map>

#include "ref_ptr.h"

class ConfSection;
class ConfValue;
typedef ref_ptr<ConfSection> ConfSectionPtr;
typedef ref_ptr<ConfValue> ConfValuePtr;
typedef std::map<std::string, ConfSectionPtr> conf_sections_t;
typedef std::map<std::string, ConfValuePtr> conf_values_t;

/* ---------------------------------------------------------------- */

class ConfValue
{
  private:
    std::string value;

  protected:
    ConfValue (void) {}

  public:
    static ConfValuePtr create (void);
    static ConfValuePtr create (const std::string& _value);

    std::string& get_string (void);
    double get_double (void);
    int get_int (void);
    bool get_bool (void);

    std::string& operator* (void);

    void set (const std::string& value);
    void set (int value);
    void set (double value);
    void set (bool value);
};

/* ---------------------------------------------------------------- */

class ConfSection
{
  private:
    conf_sections_t sections;
    conf_values_t values;
    bool do_stream;

  protected:
    ConfSection (void) { this->do_stream = true; }

  public:
    static ConfSectionPtr create (void);

    /* Adding and getting. */
    void add (const std::string& key, ConfSectionPtr section);
    void add (const std::string& key, ConfValuePtr value);
    ConfSectionPtr get_section (const std::string& key);
    ConfValuePtr get_value (const std::string& key);

    /* Iteration. */
    conf_values_t::iterator values_begin (void);
    conf_values_t::iterator values_end (void);
    conf_sections_t::iterator sections_begin (void);
    conf_sections_t::iterator sections_end (void);

    /* Clearing. */
    void clear_values (void);
    void clear_sections (void);

    /* Output to stream (file, stdout, etc). */
    void set_do_stream (bool value = true);
    void to_stream (std::ostream& outstr, std::string prefix = "");
};

/* ---------------------------------------------------------------- */

class Conf
{
  private:
    ConfSectionPtr root;

    void clip_string (std::string& str);

  public:
    Conf (void);

    ConfValuePtr get_value (const std::string& key);
    ConfSectionPtr get_section (const std::string& key);
    ConfSectionPtr get_or_create_section (const std::string& key);

    void clear (void);
    void add_from_file (std::istream& instr);
    void add_from_file (const std::string& filename);
    void add_from_string (const std::string& conf_string);
    void to_file (const std::string& filename);
    void to_stream (std::ostream& outstr);
};

/* ---------------------------------------------------------------- */

class ConfHelpers
{
  public:
    static double get_double_from_string (std::string const& str);
    static int get_int_from_string (std::string const& str);
    static std::string get_string_from_int (int val);
    static std::string get_string_from_double (double val);
};

/* ---------------------------------------------------------------- */

inline std::string&
ConfValue::get_string (void)
{
  return this->value;
}

inline std::string&
ConfValue::operator* (void)
{
  return this->value;
}

inline void
ConfValue::set (const std::string& _value)
{
  this->value = _value;
}

inline conf_values_t::iterator
ConfSection::values_begin (void)
{
  return this->values.begin();
}

inline conf_values_t::iterator
ConfSection::values_end (void)
{
  return this->values.end();
}

inline conf_sections_t::iterator
ConfSection::sections_begin (void)
{
  return this->sections.begin();
}

inline conf_sections_t::iterator
ConfSection::sections_end (void)
{
  return this->sections.end();
}

inline void
ConfSection::clear_values (void)
{
  this->values.clear();
}

inline void
ConfSection::clear_sections (void)
{
  this->sections.clear();
}

#endif /* CONF_HEADER */
