#include <iomanip>
#include <sstream>
#include <locale>

#include "helpers.h"

std::string
Helpers::get_string_from_int (int value)
{
  std::stringstream ss;
  ss << value;
  return ss.str();
}

/* ---------------------------------------------------------------- */

std::string
Helpers::get_string_from_uint (unsigned int value)
{
  std::stringstream ss;
  ss << value;
  return ss.str();
}

/* ---------------------------------------------------------------- */

std::string
Helpers::get_string_from_float (float value, int digits)
{
  std::stringstream ss;
  ss << std::fixed << std::setprecision(digits) << value;
  return ss.str();

}

/* ---------------------------------------------------------------- */

std::string
Helpers::get_string_from_double (double value, int digits)
{
  std::stringstream ss;
  ss << std::fixed << std::setprecision(digits) << value;
  return ss.str();

}

/* ---------------------------------------------------------------- */

int
Helpers::get_int_from_string (std::string const& value)
{
  std::stringstream ss(value);
  int ret;
  ss >> ret;
  return ret;
}

/* ---------------------------------------------------------------- */

std::string
Helpers::get_roman_from_int (int value)
{
  switch (value)
  {
    case 0:
    default:
      return "?";
    case 1:
      return "I";
    case 2:
      return "II";
    case 3:
      return "III";
    case 4:
      return "IV";
    case 5:
      return "V";
  }
}

/* ---------------------------------------------------------------- */

std::string
Helpers::get_dotted_str_from_int (int value)
{
  std::stringstream ss;
  ss << value;
  return Helpers::get_dotted_str_from_str(ss.str());
}

/* ---------------------------------------------------------------- */

std::string
Helpers::get_dotted_str_from_uint (unsigned int value)
{
  std::stringstream ss;
  ss << value;
  return Helpers::get_dotted_str_from_str(ss.str());
}

/* ---------------------------------------------------------------- */

std::string
Helpers::get_dotted_str_from_str (std::string const& str)
{
  std::string ret;

  int cnt = 0;
  for (int i = str.size() - 1; i >= 0; --i)
  {
    if (cnt % 3 == 0 && cnt > 0)
      ret.insert(ret.begin(), 1, ',');
    ret.insert(ret.begin(), 1, str[i]);
    cnt += 1;
  }
  return ret;
}

/* ---------------------------------------------------------------- */

std::string
Helpers::get_dotted_isk (std::string const& isk_string)
{
  size_t pos = isk_string.find_first_of('.');
  if (pos == std::string::npos)
    return isk_string;

  std::string tmp = isk_string.substr(0, pos);
  tmp = Helpers::get_dotted_str_from_str(tmp);
  tmp += isk_string.substr(pos);

  return tmp;
}

/* ---------------------------------------------------------------- */

StringVector
Helpers::split_string (const std::string& str, char delim)
{
  StringVector parts;

  unsigned int last = 0;
  unsigned int cur = 0;
  for (; cur < str.size(); ++cur)
    if (str[cur] == delim)
      {
        parts.push_back(str.substr(last, cur - last));
	last = cur + 1;
      }
  if (last < str.size())
    parts.push_back(str.substr(last));

  return parts;
}

/* ---------------------------------------------------------------- */

StringVector
Helpers::tokenize_cmd (std::string const& str)
{
  std::vector<std::string> result;

  /* Tokenize command. Delimiter is ' ', remove and handle '"' gracefully. */
  bool in_quote = false;
  std::string token;
  for (unsigned int i = 0; i < str.size(); ++i)
  {
    char chr = str[i];

    if (chr == ' ' && !in_quote)
    {
      result.push_back(token);
      token.clear();
    }
    else if (chr == '"')
      in_quote = !in_quote;
    else
      token += chr;
  }
  result.push_back(token);

  return result;
}
