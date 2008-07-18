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

  AsyncHttpData ret;

  this->busy = true;

  try
  {
    HttpDataPtr data = fetcher->request();
    ret.data = data;
  }
  catch (Exception& e)
  {
    ret.data.reset();
    ret.exception = e;
    this->sig_done.emit(ret);
  }

  delete fetcher;

  this->busy = false;

  this->sig_done.emit(ret);
}

/* ---------------------------------------------------------------- */

void
EveApiFetcher::async_request (void)
{
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
  this->sig_done.emit(data);
}
