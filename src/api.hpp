#pragma once

#include <json.hpp>
using nlohmann::json;

#include <string>
#include <utility>
#include <iostream>
#include <memory>
#include <vector>
#include <map>
using namespace std;

#ifndef FORMAT_HEADER
#define FORMAT_HEADER
#include <fmt/format.h>
#include <fmt/format.cc>
#endif
using namespace fmt;

#include <range/v3/all.hpp>
using namespace ranges;

#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/StreamCopier.h>
#include <Poco/Path.h>
#include <Poco/URI.h>
#include <Poco/Exception.h>
using namespace Poco;
using namespace Poco::Net;

namespace api {
  typedef map<string, string> Map;

  const Context::Ptr context = new Context(Context::CLIENT_USE, "", "", "", Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");

  const char* domain = getenv("FOREX_DOMAIN");
  const char* access_token = getenv("FOREX_ACCESS_TOKEN");
  const char* account_id = getenv("FOREX_ACCOUNT_ID");

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

  auto get(const string &url, const Map &params) -> json {
    pre_check();

    try {
      // prepare session
      URI uri(format("{0}/v3/{1}{2}", domain, url, flatten_params(params)));
      HTTPSClientSession session(uri.getHost(), uri.getPort(), context);
      session.setKeepAlive(true);

      // prepare path
      string path(uri.getPathAndQuery());
      if (path.empty()) path = "/";

      // send request
      HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
      req.set("Authorization", std::string("Bearer ") + access_token);
      session.sendRequest(req);

      // get response
      HTTPResponse res;
      std::istream& is = session.receiveResponse(res);
      stringstream ss;
      StreamCopier::copyStream(is, ss);

      if (res.getStatus() < 400) {
        json j = json::parse(ss.str());
        return j;
      } else {
        cout << res.getStatus() << " " << res.getReason() << endl;
        cout << ss.str() << endl;
        return nullptr;
      }
    } catch (const Exception &e) {
      cerr << e.displayText() << endl;
      return nullptr;
    }
  }

  auto get(const string &url) -> json {
    return get(url, Map({}));
  }

  auto effect_request(const string &method, const string &url, const json &body_json) -> json {
    pre_check();

    try {
      // prepare session
      URI uri(format("{0}/v3/{1}", domain, url));
      HTTPSClientSession session(uri.getHost(), uri.getPort(), context);

      // prepare path
      string path(uri.getPathAndQuery());
      if (path.empty()) path = "/";

      // send request
      HTTPRequest req(method, path, HTTPMessage::HTTP_1_1);
      req.set("Authorization", std::string("Bearer ") + access_token);

      if (body_json != nullptr) {
        auto body = body_json.dump();
        // Set the request body
        req.setContentType("application/json");
        req.setContentLength(body.length());

        // sends request, returns open stream
        std::ostream& os = session.sendRequest(req);
        os << body;  // sends the body
      } else {
        session.sendRequest(req);
      }

      // get response
      HTTPResponse res;
      std::istream &is = session.receiveResponse(res);
      stringstream ss;
      StreamCopier::copyStream(is, ss);

      if (res.getStatus() < 400) {
        json j = json::parse(ss.str());
        return j;
      } else {
        cout << res.getStatus() << " " << res.getReason() << endl;
        cout << ss.str() << endl;
        return nullptr;
      }

    } catch (const Exception &e) {
      cerr << e.displayText() << endl;
      return nullptr;
    }
  }

  auto post(const string &url, const json &body_json) -> json {
    return effect_request(HTTPRequest::HTTP_POST, url, body_json);
  }

  auto post(const string &url) -> json {
    return post(url, nullptr);
  }

  auto put(const string &url, const json &body_json) -> json {
    return effect_request(HTTPRequest::HTTP_PUT , url, body_json);
  }

  auto put(const string &url) -> json {
    return put(url, nullptr);
  }
}
