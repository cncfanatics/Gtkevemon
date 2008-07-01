#include <cerrno>
#include <cstring>
#include <iostream>

#include "config.h"
#include "exception.h"
#include "http.h"
#include "eveapi.h"

std::string
EveApi::get_post_data (EveApiAuth const& auth)
{
  std::string ret;
  ret += "userid=";
  ret += auth.user_id;
  ret += "&apiKey=";
  ret += auth.api_key;

  if (!auth.char_id.empty())
  {
    ret += "&characterID=";
    ret += auth.char_id;
  }

  return ret;
}

/* ---------------------------------------------------------------- */

void
EveApi::set_proxy_server (Http& request)
{
  ConfSectionPtr section = Config::conf.get_section("network");
  if (section->get_value("use_proxy")->get_bool())
  {
    request.set_proxy(section->get_value("proxy_address")->get_string(),
        section->get_value("proxy_port")->get_int());
  }
}

/* ---------------------------------------------------------------- */

HttpDocPtr
EveApi::request_charlist (EveApiAuth const& auth)
{
  std::cout << "Requesting XML: Characters.xml ..." << std::endl;

  Http req("api.eve-online.com", "/account/Characters.xml.aspx");
  EveApi::set_proxy_server(req);
  req.set_data(HTTP_METHOD_POST, EveApi::get_post_data(auth));
  req.set_agent("GtkEveMon");
  HttpDocPtr doc = req.request();

  //std::cout << "Successfully retrieved character list" << std::endl;

  return doc;
}

/* ---------------------------------------------------------------- */

HttpDocPtr
EveApi::request_charsheet (EveApiAuth const& auth)
{
  std::cout << "Requesting XML: CharacterSheet.xml ..." << std::endl;

  Http req("api.eve-online.com", "/char/CharacterSheet.xml.aspx");
  EveApi::set_proxy_server(req);
  req.set_data(HTTP_METHOD_POST, EveApi::get_post_data(auth));
  req.set_agent("GtkEveMon");
  HttpDocPtr doc = req.request();

  //std::cout << "Successfully retrieved character sheet" << std::endl;

  return doc;
}

/* ---------------------------------------------------------------- */

HttpDocPtr
EveApi::request_in_training (EveApiAuth const& auth)
{
  std::cout << "Requesting XML: SkillInTraining.xml ..." << std::endl;

  Http req("api.eve-online.com", "/char/SkillInTraining.xml.aspx");
  EveApi::set_proxy_server(req);
  req.set_data(HTTP_METHOD_POST, EveApi::get_post_data(auth));
  req.set_agent("GtkEveMon");
  HttpDocPtr doc = req.request();

  //std::cout << "Successfully retrieved skill in training" << std::endl;

  return doc;
}
