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

#include <string>

#include "ref_ptr.h"
#include "apibase.h"

class ApiItemTree;
typedef ref_ptr<ApiItemTree> ApiItemTreePtr;

class ApiItemTree : public ApiBase
{
  private:
    static ApiItemTreePtr instance;

  protected:
    ApiItemTree (void);
    void parse_xml (std::string const& doc);
    void parse_root_tag (xmlNodePtr node);

  public:
    int version;

  public:
    static ApiItemTreePtr request (void);
    void refresh (void);
};

#endif /* API_ITEM_TREE_HEADER */
