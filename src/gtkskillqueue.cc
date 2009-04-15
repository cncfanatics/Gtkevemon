#include <gtkmm/messagedialog.h>
#include <gtkmm/scrolledwindow.h>

#include "config.h"
#include "evetime.h"
#include "helpers.h"
#include "imagestore.h"
#include "gtkhelpers.h"
#include "gtkdefines.h"
#include "apiskilltree.h"
#include "apiskillqueue.h"
#include "gtkskillqueue.h"

GtkSkillQueueViewCols::GtkSkillQueueViewCols (Gtk::TreeView* view,
    GtkSkillQueueColumns* cols)
  : GtkColumnsBase(view),
    position("Position", cols->queue_pos),
    start_sp("Start SP", cols->start_sp),
    end_sp("Dest SP", cols->end_sp),
    start_time("Start time", cols->start_time),
    end_time("Finish time", cols->end_time),
    duration("Duration", cols->duration),
    training("Training", cols->training)
{
  this->skill_name.set_title("Skill name");
  this->skill_name.pack_start(cols->skill_icon, false);
  this->skill_name.pack_start(cols->skill_name, true);

  this->position.set_resizable(false);

  this->append_column(&this->position,
      GtkColumnOptions(false, true, true, ImageStore::columnconf[1]));
  this->append_column(&this->skill_name, GtkColumnOptions(false, false, true));
  this->append_column(&this->start_sp, GtkColumnOptions(true, false, true));
  this->append_column(&this->end_sp, GtkColumnOptions(true, false, true));
  this->append_column(&this->start_time, GtkColumnOptions(true, false, true));
  this->append_column(&this->end_time, GtkColumnOptions(true, false, true));
  this->append_column(&this->duration, GtkColumnOptions(true, false, true));
  this->append_column(&this->training, GtkColumnOptions(true, false, true));

  this->position.get_first_cell_renderer()->set_property("xalign", 1.0f);
  this->skill_name.set_expand(true);
  this->start_sp.get_first_cell_renderer()->set_property("xalign", 1.0f);
  this->end_sp.get_first_cell_renderer()->set_property("xalign", 1.0f);
}

/* ================================================================ */

GtkSkillQueue::GtkSkillQueue (void)
  : queue_store(Gtk::ListStore::create(queue_cols)),
    queue_view(queue_store),
    queue_view_cols(&queue_view, &queue_cols)
{
  /* Setup EVE API fetcher. */
  this->queue_fetcher.set_doctype(EVE_API_DOCTYPE_SKILLQUEUE);
  this->queue_fetcher.signal_done().connect(sigc::mem_fun
      (*this, &GtkSkillQueue::on_apidata_available));

  Gtk::ScrolledWindow* scwin = MK_SCWIN;
  scwin->add(this->queue_view);

  this->queue_view_cols.position.signal_clicked().connect(sigc::mem_fun
      (this->queue_view_cols, &GtkColumnsBase::toggle_edit_context));

  this->set_border_width(5);
  this->pack_start(*scwin, true, true, 0);

  //this->queue_view_cols.set_format("+0 +1 +2 +3 +4 +5 +6 +7");
  this->init_from_config();
  this->queue_view_cols.setup_columns_normal();
}

/* ---------------------------------------------------------------- */

GtkSkillQueue::~GtkSkillQueue (void)
{
  this->store_to_config();
  Config::save_to_file();
}

/* ---------------------------------------------------------------- */

void
GtkSkillQueue::refresh (void)
{
  this->queue_fetcher.async_request();
}

/* ---------------------------------------------------------------- */

void
GtkSkillQueue::init_from_config (void)
{
  ConfSectionPtr skillqueue = Config::conf.get_section("skillqueue");
  ConfValuePtr columns_format = skillqueue->get_value("columns_format");
  this->queue_view_cols.set_format(**columns_format);
}

/* ---------------------------------------------------------------- */

void
GtkSkillQueue::store_to_config (void)
{
  ConfSectionPtr skillqueue = Config::conf.get_section("skillqueue");
  ConfValuePtr columns_format = skillqueue->get_value("columns_format");
  columns_format->set(this->queue_view_cols.get_format());
}

/* ---------------------------------------------------------------- */

void
GtkSkillQueue::on_apidata_available (EveApiData data)
{
  ApiSkillQueuePtr sq = ApiSkillQueue::create();
  try
  {
    sq->set_api_data(data);
    if (sq->is_locally_cached())
      this->raise_error(data.exception, true);
  }
  catch (Exception& e)
  {
    this->raise_error(e, false);
    return;
  }

  /* Debugging. */
  //sq->debug_dump();

  /* FIXME: Pay attention to training times. */

  this->queue_store->clear();
  ApiSkillTreePtr tree = ApiSkillTree::request();
  time_t training = 0;
  for (std::size_t i = 0; i < sq->queue.size(); ++i)
  {
    ApiSkillQueueItem const& item = sq->queue[i];
    ApiSkill const* skill = tree->get_skill_for_id(item.skill_id);

    time_t duration = item.end_time_t - item.start_time_t;
    training += duration;

    std::string skill_name = skill->name;
    skill_name += " ";
    skill_name += Helpers::get_roman_from_int(item.to_level);

    Gtk::TreeModel::Row row = *this->queue_store->append();
    row[this->queue_cols.queue_pos] = item.queue_pos;
    row[this->queue_cols.skill_name] = skill_name;
    row[this->queue_cols.skill_icon] = ImageStore::skillicons[1];
    row[this->queue_cols.start_sp] = item.start_sp;
    row[this->queue_cols.end_sp] = item.end_sp;
    row[this->queue_cols.start_time]
        = EveTime::get_local_time_string(item.start_time_t, true);
    row[this->queue_cols.end_time]
        = EveTime::get_local_time_string(item.end_time_t, true);
    row[this->queue_cols.duration]
        = EveTime::get_string_for_timediff(duration, true);
    row[this->queue_cols.training]
        = EveTime::get_string_for_timediff(training, true);
  }
}

/* ---------------------------------------------------------------- */

void
GtkSkillQueue::raise_error (std::string const& error, bool cached)
{
  Gtk::MessageType message_type;
  Glib::ustring message;
  if (cached)
  {
    message = "Using cached version of the skill queue!";
    message_type = Gtk::MESSAGE_WARNING;
  }
  else
  {
    message = "Error retrieving skill queue!";
    message_type = Gtk::MESSAGE_ERROR;
  }

  Gtk::Window* win = (Gtk::Window*)this->get_toplevel();
  Gtk::MessageDialog md(*win, message, false, message_type, Gtk::BUTTONS_OK);
  md.set_secondary_text("There was an error while requesting the skill "
      "queue from the EVE API. The EVE API is either offline, or the "
      "requested document is not understood by GtkEveMon. "
      "The error message is:\n\n" + GtkHelpers::locale_to_utf8(error));
  md.set_title("Error - GtkEveMon");
  md.run();
}
