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

#ifndef API_ITEM_TREE_HEADER
#define API_ITEM_TREE_HEADER

#include <map>
#include <string>
#include <vector>

#include "ref_ptr.h"
#include "apibase.h"

enum ApiItemRace
{
  API_RACE_CALDARI = 1 << 0,
  API_RACE_MINMATAR = 1 << 1,
  API_RACE_AMARR = 1 << 2,
  API_RACE_GALLENTE = 1 << 3,
  API_RACE_JOVE = 1 << 4,
  API_RACE_PIRATE = 1 << 5
};

/* ---------------------------------------------------------------- */

typedef std::vector<std::pair<std::string, float> > ApiItemPropertyList;
typedef std::vector<std::pair<int, int> > ApiItemSkillDeps;

class ApiItem
{
  public:
    int id;
    std::string name;
    std::string iconname;
    std::string description;

    ApiItemPropertyList properties;
    ApiItemSkillDeps skilldeps;
};

/* ---------------------------------------------------------------- */

class ApiItemCategory;
typedef std::map<int, ApiItemCategory> ApiItemCategoryList;
typedef std::map<int, ApiItem> ApiItemList;

class ApiItemCategory
{
  public:
    int id;
    std::string name;
    std::string iconname;
    std::string description;

    ApiItemCategoryList subcats;
    ApiItemList items;
};

/* ---------------------------------------------------------------- */

class ApiItemTree;
typedef ref_ptr<ApiItemTree> ApiItemTreePtr;

class ApiItemTree : public ApiBase
{
  private:
    static ApiItemTreePtr instance;

  protected:
    ApiItemTree (void);
    void parse_xml (std::string const& filename);
    void parse_root_tag (xmlNodePtr node);
    void parse_item_categories (ApiItemCategoryList& cats, xmlNodePtr node);
    void parse_item_category (ApiItemCategory& cat, xmlNodePtr node);
    void parse_item_list (ApiItemCategory& cat, xmlNodePtr node);
    void parse_item (ApiItem& item, xmlNodePtr node);

  public:
    int version;
    ApiItemCategoryList cats;

  public:
    static ApiItemTreePtr request (void);
    void refresh (void);

    void debug_dump (ApiItemCategoryList* cats = 0, int indent = 0);
    void debug_dump (ApiItemList* items, int indent);
};

#endif /* API_ITEM_TREE_HEADER */
