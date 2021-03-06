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

#ifndef API_CERT_TREE_HEADER
#define API_CERT_TREE_HEADER

#include <vector>
#include <string>
#include <map>
#include <libxml/parser.h>

#include "ref_ptr.h"
#include "apibase.h"

struct ApiCertCategory
{
  int id;
  std::string name;
};

/* ---------------------------------------------------------------- */

struct ApiCertClass
{
  int id;
  std::string name;
  ApiCertCategory const* cat_details;
};

/* ---------------------------------------------------------------- */

class ApiCert : public ApiElement
{
  public:
    int id;
    int grade;
    std::string desc;
    ApiCertClass const* class_details;

    std::vector<std::pair<int, int> > skilldeps;
    std::vector<std::pair<int, int> > certdeps;

  public:
    ~ApiCert (void) {}
    ApiElementType get_type (void) const;
    void debug (void) const;
};

/* ---------------------------------------------------------------- */

class ApiCertTree;
typedef ref_ptr<ApiCertTree> ApiCertTreePtr;
typedef std::map<int, ApiCert> ApiCertMap;
typedef std::map<int, ApiCertCategory> ApiCertCategoryMap;
typedef std::map<int, ApiCertClass> ApiCertClassMap;

class ApiCertTree : public ApiBase
{
  private:
    static ApiCertTreePtr instance;

  protected:
    ApiCertTree (void);
    void parse_xml (std::string const& filename);
    void parse_eveapi_tag (xmlNodePtr node);
    void parse_result_tag (xmlNodePtr node);
    void parse_categories_rowset (xmlNodePtr node);
    void parse_categories_row (ApiCertCategory* category, xmlNodePtr node);
    void parse_classes_rowset (ApiCertCategory* category, xmlNodePtr node);
    void parse_classes_row (ApiCertClass* cclass, xmlNodePtr node);
    void parse_certificates_rowset (ApiCertClass* cclass, xmlNodePtr node);
    void parse_certificate_row (ApiCert* cert, xmlNodePtr node);

  public:
    ApiCertMap certificates;
    ApiCertCategoryMap categories;
    ApiCertClassMap classes;
    int version;

  public:
    static ApiCertTreePtr request (void);

    ApiCertClass const* get_class_for_id (int id) const;
    ApiCertCategory const* get_category_for_id (int id) const;
    ApiCert const* get_certificate_for_id (int id) const;

    static char const* get_name_for_grade (int grade);
    static int get_grade_index (int grade);

    void debug_dump (void);
    void refresh (void);
};

/* ---------------------------------------------------------------- */

inline ApiElementType
ApiCert::get_type (void) const
{
  return API_ELEM_CERT;
}

#endif /* API_CERT_TREE_HEADER */
