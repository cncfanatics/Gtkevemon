#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>

#include "config.h"
#include "eveapi.h"

EveApiFetcher::~EveApiFetcher (void)
{
  this->conn_sigdone.disconnect();
}

/* ---------------------------------------------------------------- */

AsyncHttp*
EveApiFetcher::setup_fetcher (void)
{
  /* Setup HTTP fetcher. */
  AsyncHttp* fetcher = AsyncHttp::create();
  fetcher->set_host("api.eve-online.com");
  fetcher->set_agent("GtkEveMon");

  /* Setup HTTP post data. */
  std::string post_data;
  post_data += "userid=";
  post_data += this->auth.user_id;
  post_data += "&apiKey=";
  post_data += this->auth.api_key;
  if (!auth.char_id.empty())
  {
    post_data += "&characterID=";
    post_data += this->auth.char_id;
  }
  fetcher->set_data(HTTP_METHOD_POST, post_data);

  /* Setup proxy if set. */
  ConfSectionPtr section = Config::conf.get_section("network");
  if (section->get_value("use_proxy")->get_bool())
  {
    fetcher->set_proxy(section->get_value("proxy_address")->get_string(),
        (uint16_t)section->get_value("proxy_port")->get_int());
  }

  switch (this->type)
  {
    case EVE_API_DOCTYPE_CHARLIST:
      fetcher->set_path("/account/Characters.xml.aspx");
      break;
    case EVE_API_DOCTYPE_CHARSHEET:
      fetcher->set_path("/char/CharacterSheet.xml.aspx");
      break;
    case EVE_API_DOCTYPE_INTRAINING:
      fetcher->set_path("/char/SkillInTraining.xml.aspx");
      break;
    default:
      delete fetcher;
      std::cout << "Bug: Invalid API document type" << std::endl;
      return 0;
  }

  return fetcher;
}

/* ---------------------------------------------------------------- */

void
EveApiFetcher::request (void)
{
  AsyncHttp* fetcher = this->setup_fetcher();
  if (fetcher == 0)
    return;

  EveApiData ret;

  this->busy = true;

  try
  {
    HttpDataPtr data = fetcher->request();
    ret.data = data;
  }
  catch (Exception& e)
  {
    delete fetcher;
    ret.data.reset();
    ret.exception = e;
    this->process_caching(ret);
    this->sig_done.emit(ret);
    return;
  }

  delete fetcher;

  this->busy = false;

  this->process_caching(ret);
  this->sig_done.emit(ret);
}

/* ---------------------------------------------------------------- */

void
EveApiFetcher::async_request (void)
{
  std::cout << "Request XML: " << this->get_doc_name() << " ..." << std::endl;

  AsyncHttp* fetcher = this->setup_fetcher();
  if (fetcher == 0)
    return;

  this->busy = true;

  this->conn_sigdone = fetcher->signal_done().connect(sigc::mem_fun
      (*this, &EveApiFetcher::async_reply));
  fetcher->async_request();
}

/* ---------------------------------------------------------------- */

void
EveApiFetcher::async_reply (AsyncHttpData data)
{
  this->busy = false;
  EveApiData apidata(data);
  this->process_caching(apidata);
  this->sig_done.emit(apidata);
}

/* ---------------------------------------------------------------- */

void
EveApiFetcher::process_caching (EveApiData& data)
{
  /* Generate filename to use as cache. */
  std::string xmlname = this->get_doc_name();
  std::string path = Config::get_conf_dir();
  path += "/sheets";
  std::string file = path;
  file += "/";
  switch (this->type)
  {
    case EVE_API_DOCTYPE_CHARLIST:
      file += this->auth.user_id;
      break;
    case EVE_API_DOCTYPE_INTRAINING:
    case EVE_API_DOCTYPE_CHARSHEET:
      file += this->auth.char_id;
      break;
    default:
      std::cout << "Error: Invalid API document type!" << std::endl;
      return;
  }
  file += "_";
  file += xmlname;

  if (data.data.get() != 0)
  {
    /* Cache successful request to file. */
    //std::cout << "Should cache to file: " << file << std::endl;
    int dir_exists = ::access(path.c_str(), F_OK);
    if (dir_exists < 0)
    {
      int ret = ::mkdir(path.c_str(), S_IRWXU);
      if (ret < 0)
      {
        std::cout << "Error: Couldn't create the cache directory: "
            << ::strerror(errno) << std::endl;
        return;
      }
    }

    /* Write the file. */
    std::ofstream out(file.c_str());
    if (out.fail())
    {
      std::cout << "Error: Couldn't write to cache file!" << std::endl;
      return;
    }
    std::cout << "Caching XML: " << xmlname << " ..." << std::endl;
    out.write(data.data->data, data.data->size);
    out.close();
  }
  else
  {
    /* Read unsuccessful requests from cache if available. */
    //std::cout << "Should read from cache: " << file << std::endl;
    int file_exists = ::access(file.c_str(), F_OK);
    if (file_exists < 0)
    {
      std::cout << "Warning: No cache file for "
          << xmlname << std::endl;
      return;
    }

    /* Read from file. */
    std::string input;
    std::ifstream in(file.c_str());
    while (!in.eof())
    {
      std::string line;
      std::getline(in, line);
      input += line;
    }
    in.close();

    data.data = HttpData::create(input.size());
    data.locally_cached = true;
    ::memcpy(data.data->data, input.c_str(), input.size());

    std::cout << "Warning: Using " << xmlname << " from cache!" << std::endl;
  }
}

/* ---------------------------------------------------------------- */

char const*
EveApiFetcher::get_doc_name (void)
{
  switch (this->type)
  {
    case EVE_API_DOCTYPE_CHARLIST:
      return "Characters.xml";
    case EVE_API_DOCTYPE_INTRAINING:
      return "SkillInTraining.xml";
    case EVE_API_DOCTYPE_CHARSHEET:
      return "CharacterSheet.xml";
    default:
      return "Unknown";
  }
}
