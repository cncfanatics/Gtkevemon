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

#ifndef XML_TRAINING_PLAN_HEADER
#define XML_TRAINING_PLAN_HEADER

#include <string>
#include <vector>

#include "xml.h"
#include "apiskilltree.h"

struct XmlTrainingItem
{
  ApiSkill const* skill;
  unsigned int level;
  bool prerequisite;

  XmlTrainingItem (void);
  XmlTrainingItem (ApiSkill const* skill, unsigned int level, bool prereq);
};

typedef std::vector<XmlTrainingItem> TrainingPlan;

/* ---------------------------------------------------------------- */

class XmlTrainingPlanImport : public XmlBase
{
  private:
    TrainingPlan plan;

  protected:
    void parse_xml (XmlDocumentPtr xmldoc);

  public:
    XmlTrainingPlanImport (void);

    void import_from_file (std::string const& filename);

    TrainingPlan& get_training_plan (void);
    TrainingPlan const& get_training_plan (void) const;
};

/* ---------------------------------------------------------------- */

class XmlTrainingPlanExport
{
  private:
    TrainingPlan plan;

  public:
    XmlTrainingPlanExport (void);

    void append_training_item (XmlTrainingItem const& item);
    void write_to_file (std::string const& filename);
};

/* ---------------------------------------------------------------- */

inline
XmlTrainingItem::XmlTrainingItem (void)
{
}

inline
XmlTrainingItem::XmlTrainingItem (ApiSkill const* skill,
    unsigned int level, bool prereq)
  : skill(skill), level(level), prerequisite(prereq)
{
}

inline
XmlTrainingPlanImport::XmlTrainingPlanImport (void)
{
}

inline TrainingPlan&
XmlTrainingPlanImport::get_training_plan (void)
{
  return this->plan;
}

inline TrainingPlan const&
XmlTrainingPlanImport::get_training_plan (void) const
{
  return this->plan;
}

inline
XmlTrainingPlanExport::XmlTrainingPlanExport (void)
{
}

inline void
XmlTrainingPlanExport::append_training_item (XmlTrainingItem const& item)
{
  this->plan.push_back(item);
}

#endif /* XML_TRAINING_PLAN_HEADER */
