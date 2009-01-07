#ifndef GTK_DOWNLOADER_HEADER
#define GTK_DOWNLOADER_HEADER

#include <string>
#include <vector>
#include <gtkmm/label.h>
#include <gtkmm/frame.h>
#include <gtkmm/progressbar.h>

#include "asynchttp.h"

/* The progress is updated every this milli seconds. */
#define GTK_DOWNLOADER_GUI_UPDATE 100

struct DownloadItem
{
  std::string name;
  std::string url;

  DownloadItem (void);
  DownloadItem (std::string const& name, std::string const& url);
};

/* ---------------------------------------------------------------- */

typedef sigc::signal<void, DownloadItem, AsyncHttpData> SignalDownloadDone;
typedef sigc::signal<void> SignalAllDownloadsDone;

class GtkDownloader : public Gtk::Frame
{
  private:
    std::vector<DownloadItem> downloads;
    SignalDownloadDone sig_download_done;
    SignalAllDownloadsDone sig_all_downloads_done;

    Gtk::Label filename_label;
    Gtk::ProgressBar progressbar;

    AsyncHttp* asynchttp;

  protected:
    void start_next_download (void);
    void on_cancel_clicked (void);
    void on_download_complete (AsyncHttpData data);
    bool on_update_event (void);

  public:
    GtkDownloader (void);

    void append_download (DownloadItem const& item);
    void start_downloads (void);
    void cancel_downloads (void);

    SignalDownloadDone& signal_download_done (void);
    SignalAllDownloadsDone& signal_all_downloads_done (void);
};

/* ---------------------------------------------------------------- */

inline
DownloadItem::DownloadItem (void)
{
}

inline
DownloadItem::DownloadItem (std::string const& name, std::string const& url)
{
  this->name = name;
  this->url = url;
}

inline void
GtkDownloader::append_download (DownloadItem const& item)
{
  this->downloads.push_back(item);
}

inline SignalDownloadDone&
GtkDownloader::signal_download_done (void)
{
  return this->sig_download_done;
}

inline SignalAllDownloadsDone&
GtkDownloader::signal_all_downloads_done (void)
{
  return this->sig_all_downloads_done;
}

#endif /* GTK_DOWNLOADER_HEADER */
