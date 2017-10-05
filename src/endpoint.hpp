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

namespace endpoint {
  typedef map<string, string> Map;

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
      const Context::Ptr context = new Context(Context::CLIENT_USE, "", "", "", Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");

      // prepare session
      URI uri(format("{0}{1}", url, flatten_params(params)));
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
        cout << res.getStatus() << endl;
        cout << ss.str() << endl;
        return nullptr;
      }
    } catch (const Exception &e) {
      cerr << e.displayText() << endl;
      return nullptr;
    }
  }
}
