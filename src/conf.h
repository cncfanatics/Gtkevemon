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
    void remove_section (std::string const& key);
    void remove_value (std::string const& key);

    /* Iteration. */
    conf_values_t::iterator values_begin (void);
    conf_values_t::iterator values_end (void);
    conf_sections_t::iterator sections_begin (void);
    conf_sections_t::iterator sections_end (void);

    conf_values_t::iterator find_value (std::string const& key);
    conf_sections_t::iterator find_section (std::string const& key);

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

inline conf_values_t::iterator
ConfSection::find_value (std::string const& key)
{
  return this->values.find(key);
}

inline conf_sections_t::iterator
ConfSection::find_section (std::string const& key)
{
  return this->sections.find(key);
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
