/*
 * conf.cc -- Part of the SimFM filemanager
 * Copyright (c) 2004 by Simon Fuhrmann
 *
 * Refer to "COPYING" for license information.
 */
#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#include "exception.h"
#include "conf.h"

/* ---------------------------------------------------------------- */

ConfValuePtr
ConfValue::create (void)
{
  return ConfValuePtr(new ConfValue);
}

/* ---------------------------------------------------------------- */

ConfValuePtr
ConfValue::create (const std::string& _value)
{
  ConfValuePtr c(new ConfValue);
  c->set(_value);
  return c;
}

/* ---------------------------------------------------------------- */

double
ConfValue::get_double (void)
{
  try { return ConfHelpers::get_double_from_string(this->value); }
  catch (Exception& e)
  { std::cout << "Conf: " << e << std::endl; }
  return 0;
}

/* ---------------------------------------------------------------- */

int
ConfValue::get_int (void)
{
  try { return ConfHelpers::get_int_from_string(this->value); }
  catch (Exception& e)
  { std::cout << "Conf: " << e << std::endl; }
  return 0;
}

/* ---------------------------------------------------------------- */

bool
ConfValue::get_bool (void)
{
  if (this->value == "" || this->value == "0" || this->value == "false")
    return false;
  else
    return true;
}

/* ---------------------------------------------------------------- */

void
ConfValue::set (int _value)
{
  this->value = ConfHelpers::get_string_from_int(_value);
}

/* ---------------------------------------------------------------- */

void
ConfValue::set (double _value)
{
  this->value = ConfHelpers::get_string_from_double(_value);
}

/* ---------------------------------------------------------------- */

void
ConfValue::set (bool _value)
{
  this->value = (_value ? "true" : "false");
}

/* ================================================================ */

ConfSectionPtr
ConfSection::create (void)
{
  return ConfSectionPtr(new ConfSection);
}

/* ---------------------------------------------------------------- */

void
ConfSection::add (const std::string& key, ConfSectionPtr section)
{
  if (this->sections.insert(std::make_pair(key, section)).second == false)
    this->sections.find(key)->second = section;
}

/* ---------------------------------------------------------------- */

void
ConfSection::add (const std::string& key, ConfValuePtr value)
{
  // TRY: this->values[key] = value;
  if (this->values.insert(std::make_pair(key, value)).second == false)
    this->values.find(key)->second = value;
}

/* ---------------------------------------------------------------- */

ConfSectionPtr
ConfSection::get_section (const std::string& key)
{
  size_t dot_pos = key.find_first_of('.');
  if (dot_pos == std::string::npos)
  {
    conf_sections_t::iterator iter = this->sections.find(key);
    if (iter == this->sections.end())
    {
      //std::cout << "Conf: " << key << ": Section does not exist"
      //    << std::endl;
      throw Exception("Section does not exist");
    }
    else
      return iter->second;
  }
  else
  {
    std::string section = key.substr(0, dot_pos);
    std::string new_key = key.substr(dot_pos + 1);
    conf_sections_t::iterator iter = this->sections.find(section);

    if (iter == this->sections.end())
      throw Exception("Conf: " + key + ": Path to section does not exist");
    else
      return iter->second->get_section(new_key);
  }
}

/* ---------------------------------------------------------------- */

ConfValuePtr
ConfSection::get_value (const std::string& key)
{
  conf_values_t::iterator iter = this->values.find(key);
  if (iter != this->values.end())
    return iter->second;

  size_t dot_pos = key.find_first_of('.');
  if (dot_pos == std::string::npos)
  {
    throw Exception("Key does not exist");
  }
  else
  {
    std::string section = key.substr(0, dot_pos);
    std::string new_key = key.substr(dot_pos + 1);
    conf_sections_t::iterator iter = this->sections.find(section);
    if (iter == this->sections.end())
      throw Exception("Path to key does not exist");
    else
      return iter->second->get_value(new_key);
  }
}

/* ---------------------------------------------------------------- */

void
ConfSection::remove_section (std::string const& key)
{
  this->sections.erase(key);
}

/* ---------------------------------------------------------------- */

void
ConfSection::remove_value (std::string const& key)
{
  this->values.erase(key);
}

/* ---------------------------------------------------------------- */

void
ConfSection::to_stream (std::ostream& outstr, std::string prefix)
{
  if (this->do_stream == false)
    return;

  /* Process each value in this section. */
  {
    conf_values_t::iterator iter = this->values.begin();
    while (iter != this->values.end())
    {
      if (iter->first.size() == 0)
        outstr << "  " << "= ";
      else
        outstr << "  " << iter->first << " = ";
      outstr << iter->second->get_string() << std::endl;
      iter++;
    }
    /* Print newline after each non-empty section. */
    if (this->values.size() > 0)
      outstr << std::endl;
  }

  /* Process each subsection. */
  {
    conf_sections_t::iterator iter = sections.begin();
    while (iter != this->sections.end())
    {
      if (prefix.size() == 0)
      {
        outstr << "[" << iter->first << "]" << std::endl;
        iter->second->to_stream(outstr, iter->first);
      }
      else
      {
        outstr << "[" << prefix << "." << iter->first << "]" << std::endl;
        iter->second->to_stream(outstr, prefix + "." + iter->first);
      }
      iter++;
    }
  }
}

/* ================================================================ */

Conf::Conf (void)
{
  this->clear();
}

/* ---------------------------------------------------------------- */
/* Returns value or throws if value does not exist. */

ConfValuePtr
Conf::get_value (const std::string& key)
{
  try
  { return root->get_value(key); }
  catch (Exception& e)
  {
    //std::cout << key << ": " << e << std::endl;
    throw Exception(e + ": " + key);
  }
}

/* ---------------------------------------------------------------- */
/* Returns the section or throws if section does not exist. */

ConfSectionPtr
Conf::get_section (const std::string& key)
{
  try
  { return root->get_section(key); }
  catch (Exception& e)
  {
    //std::cout << key << ": " << e << std::endl;
    throw Exception(e + ": " + key);
  }
}

/* ---------------------------------------------------------------- */
/* Returns section or creates a new if section does not exist. */

ConfSectionPtr
Conf::get_or_create_section (const std::string& key)
{
  /* Try to find already existing section. */
  try
  { return this->get_section(key); }
  catch(Exception& e)
  { }

  /* Section does not exist. */
  //std::cout << "Conf: Creating section " << key << std::endl;
  size_t dot_pos = key.find_last_of('.');
  if (dot_pos == 0 || dot_pos == key.size() - 1)
  {
    /* Report syntax error. */
    throw Exception("Syntax error: Invalid section name");
  }
  else if (dot_pos == std::string::npos)
  {
    /* Add new section to the root. */
    ConfSectionPtr new_section = ConfSection::create();
    this->root->add(key, new_section);
    return new_section;
  }
  else
  {
    /* Try to get parent section. */
    ConfSectionPtr parent;
    std::string parent_name = key.substr(0, dot_pos);
    std::string section_name = key.substr(dot_pos + 1);
    try
    {
      parent = this->get_or_create_section(parent_name);
      ConfSectionPtr new_section = ConfSection::create();
      parent->add(section_name, new_section);
      return new_section;
    }
    catch (Exception& e)
    {
      /* Parent section not found. */
      throw Exception("Semantic error: Parent section not found");
    }
  }
}

/* ---------------------------------------------------------------- */

void
Conf::clear (void)
{
  this->root = ConfSection::create();
}

/* ---------------------------------------------------------------- */

void
Conf::add_from_file (const std::string& filename)
{
  std::ifstream instr(filename.c_str());
  if (instr.fail())
  {
    std::cout << "Conf: Could not open " << filename << ": "
        << strerror(errno) << std::endl;
    return;
  }

  //std::cout << "Conf: Start parsing of " << filename << std::endl;
  this->add_from_file(instr);
  //std::cout << "Conf: Parsing done" << std::endl;
  instr.close();
}

/* ---------------------------------------------------------------- */

void
Conf::add_from_file (std::istream& instr)
{
  std::string buf;
  int buf_line = 0;
  ConfSectionPtr cur_section;
  while (!instr.eof())
  {
    std::getline(instr, buf);
    buf_line++;

    /* Clip string buffer. */
    size_t comment_pos = buf.find_first_of('#');
    if (comment_pos != std::string::npos)
      buf = buf.substr(0, comment_pos);

    this->clip_string(buf);

    if (buf.size() == 0)
      continue;

    /* Analyse content line. */
    if (buf[0] == '[' && buf[buf.size() - 1] == ']')
    {
      /* It's a new section. Clear current section. */
      cur_section = 0;

      /* Get the new sections name. */
      std::string section_name = buf.substr(1, buf.size() - 2);
      this->clip_string(section_name);
      if (section_name.size() == 0)
      {
        std::cout << "Conf: Line " << buf_line << ": Syntax error: "
            << "No config name given" << std::endl;
        continue;
      }

      /* Try to get or insert the section. */
      try
      { cur_section = this->get_or_create_section(section_name); }
      catch (Exception& e)
      {
        std::cout << "Conf: Line " << buf_line << ": " << e << std::endl;
        continue;
      }

      //std::cout << "Section inserted: " << section_name << std::endl;
    }
    else
    {
      /* There must be a current section for the value. */
      if (cur_section == 0)
      {
        std::cout << "Conf: Line " << buf_line << ": Semantic error: "
            << "Cannot insert value to invalid section" << std::endl;
        continue;
      }
      size_t separator_pos = buf.find_first_of('=');
      if (separator_pos == std::string::npos)
      {
        std::cout << "Conf: Line " << buf_line << ": Syntax error: "
            << "No key/value delimiter found" << std::endl;
        continue;
      }
      /* Extract key and value. */
      std::string key = buf.substr(0, separator_pos);
      std::string val = buf.substr(separator_pos + 1);
      this->clip_string(key);
      this->clip_string(val);
      /* Insert key-value pair. */
      ConfValuePtr value = ConfValue::create();
      value->set(val);
      cur_section->add(key, value);

      //std::cout << "Conf: Value inserted: " << key
      //    << "=" << val << std::endl;
    }
  }
  //std::cout << "Conf: Done parsing" << std::endl;
}

/* ---------------------------------------------------------------- */

void
Conf::add_from_string (const std::string& conf_string)
{
  std::stringstream conf_file;
  conf_file << conf_string;
  this->add_from_file(conf_file);
}

/* ---------------------------------------------------------------- */

void
Conf::to_file (const std::string& filename)
{
  //std::cout << "Saving configuration..." << std::endl;

  std::ofstream file(filename.c_str());
  if (file.fail())
  {
    //std::cout << "Conf: Could not open " << filename << ": "
    //    << strerror(errno) << std::endl;
    throw Exception((filename + ": ") + ::strerror(errno));
  }

  /* Print config to file. */
  this->root->to_stream(file, "");
  file.close();
}

/* ---------------------------------------------------------------- */

void
Conf::to_stream (std::ostream& outstr)
{
  this->root->to_stream(outstr);
}

/* ---------------------------------------------------------------- */

void
Conf::clip_string (std::string& str)
{
  while (!str.empty() &&
      (str[str.size() - 1] == ' ' || str[str.size() - 1] == '\t'
      || str[str.size() - 1] == '\n' || str[str.size() - 1] == '\r'))
    str.erase(str.size() - 1);

  while (!str.empty() && (str[0] == ' ' || str[0] == '\t'
      || str[0] == '\n'|| str[0] == '\r'))
    str.erase(0, 1);
}

/* ================================================================ */

double
ConfHelpers::get_double_from_string (std::string const& str)
{
  std::istringstream s(str);
  int ret;
  if (!(s >> ret) || !s.eof())
    throw Exception("Invalid int format");

  return ret;
}

/* ---------------------------------------------------------------- */

int
ConfHelpers::get_int_from_string (std::string const& str)
{
  std::istringstream s(str);
  int ret;
  if (!(s >> ret) || !s.eof())
    throw Exception("Invalid int format");

  return ret;
}

/* ---------------------------------------------------------------- */

std::string
ConfHelpers::get_string_from_int (int val)
{
  std::stringstream s;
  s << val;
  return s.str();
}

/* ---------------------------------------------------------------- */

std::string
ConfHelpers::get_string_from_double (double val)
{
  std::stringstream s;
  s << val;
  return s.str();
}

