#ifndef API_BASE_HEADER
#define API_BASE_HEADER

#include <string>
#include <libxml/parser.h>

class ApiBase
{
  protected:
    std::string cached_until;

    /* Extracts some common information like errors,
     * the EVE time and the cache time. */
    void check_node (xmlNodePtr node);

    /* Some helper functions. */
    std::string get_property (xmlNodePtr node, char const* name);
    std::string get_node_text (xmlNodePtr node);

    void set_string_if_node_text (xmlNodePtr node, char const* node_name,
        std::string& target);
    void set_int_if_node_text (xmlNodePtr node, char const* node_name,
        int& target);
    void set_bool_if_node_text (xmlNodePtr node, char const* node_name,
        bool& target);

  public:
    std::string const& get_cached_until (void) const;
};

/* ---------------------------------------------------------------- */

inline std::string const&
ApiBase::get_cached_until (void) const
{
  return this->cached_until;
}

#endif /* API_BASE_HEADER */
