#pragma once

#include <json.hpp>
using json = nlohmann::json;

#include <string>
#include <utility>
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <initializer_list>
using namespace std;

#include <cpr/cpr.h>
using namespace cpr;

#ifndef FORMAT_HEADER
#define FORMAT_HEADER
#include <fmt/format.h>
#include <fmt/format.cc>
#endif
using namespace fmt;

#include <range/v3/all.hpp>
using namespace ranges;

namespace api {
  typedef map<string, string> Map;
  typedef initializer_list<Parameter> Params;

  const char* domain = getenv("FOREX_DOMAIN");
  const char* access_token = getenv("FOREX_ACCESS_TOKEN");
  const char* account_id = getenv("FOREX_ACCOUNT_ID");

  const Header &headers = {
    { "accept", "application/json" },
    { "Authorization", format("Bearer {}", access_token) }
  };

  const Header &effect_headers = {
    { "accept", "application/json" },
    { "Content-Type", "application/json" },
    { "Authorization", "Bearer bae2d781dd63e0043d67890dad056e53-3de2bb579fd9c6f70c5dd90e1a2ddb3a" }
  };

  enum class EFFECT_TYPE : size_t {
    POST,
    PUT,
    PATCH
  };

  auto pre_check() {
    if (!(domain && access_token && account_id))
      cerr << "Please setup basic configuration!" << endl;
  }

  auto flatten_params(const Map &params) -> string {
    vector<string> params_vec = params
      | view::transform([](const auto &pair) {
          return format("{0}={1}", pair.first, pair.second);
        });
    string params_str = params_vec | view::join('&');
    if (!params_str.empty()) {
      return format("?{}", params_str);
    } else {
      return params_str;
    }
  }

  auto response_tweak(Response response) -> json {
    if (response.status_code < 400) {
      return nlohmann::json::parse(response.text);
    } else {
      cout << response.status_code << " " << response.text << endl;
      return nullptr;
    }
  }

  auto get(const string &url, const Params &params) -> json {
    pre_check();

    auto response = Get(Url{format("{0}/v3/{1}", domain, url)},
                        headers,
                        Parameters(params));
    return response_tweak(response);
  }

  auto get(const string &url) -> json {
    return get(url, Params({}));
  }

  auto effect_request(EFFECT_TYPE method, const string &url, const json &body) -> json {
    pre_check();

    Session session;
    session.SetUrl(Url{ format("{0}/v3/{1}", domain, url) });
    session.SetHeader(effect_headers);
    session.SetBody(Body(body.dump()));

    Response response;
    switch (method) {
    case EFFECT_TYPE::POST: {
      response = session.Post();
      break;
    }
    case EFFECT_TYPE::PUT: {
      response = session.Put();
      break;
    }
    case EFFECT_TYPE::PATCH: {
      response = session.Patch();
      break;
    }
    default:
      break;
    }

    return response_tweak(response);
  }

  auto post(const string &url, const json &body_json) -> json {
    return effect_request(EFFECT_TYPE::POST, url, body_json);
  }

  auto post(const string &url) -> json {
    return post(url, nullptr);
  }

  auto put(const string &url, const json &body_json) -> json {
    return effect_request(EFFECT_TYPE::PUT, url, body_json);
  }

  auto put(const string &url) -> json {
    return put(url, nullptr);
  }

  auto patch(const string &url, const json &body_json) -> json {
    return effect_request(EFFECT_TYPE::PATCH, url, body_json);
  }

  auto patch(const string &url) -> json {
    return patch(url, nullptr);
  }
}
