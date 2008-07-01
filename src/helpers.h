#ifndef HELPERS_HEADER
#define HELPERS_HEADER

#include <string>
#include <vector>

typedef std::vector<std::string> StringVector;

class Helpers
{
  public:
    static std::string get_string_from_int (int value);
    static std::string get_string_from_uint (unsigned int value);
    static std::string get_string_from_float (float value, int digits);
    static std::string get_string_from_double (double value, int digits);
    static int get_int_from_string (std::string const& value);
    static std::string get_roman_from_int (int value);

    static std::string get_dotted_str_from_int (int value);
    static std::string get_dotted_str_from_uint (unsigned int value);
    static std::string get_dotted_str_from_str (std::string const& str);
    static std::string get_dotted_isk (std::string const& isk_string);

    static StringVector split_string (std::string const& str, char delim);
    static StringVector tokenize_cmd (std::string const& str);
};

#endif /* HELPERS_HEADER */
