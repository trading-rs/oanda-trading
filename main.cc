#include <json.hpp>
#include <string>
#include <utility>
#include <iostream>
#include <memory>
#include <vector>
#include <map>

#ifndef FORMAT_HEADER
#define FORMAT_HEADER
#include <fmt/format.h>
#include <fmt/format.cc>
#endif

#include <range/v3/all.hpp>

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
using namespace std;
using namespace fmt;
using namespace ranges;
using nlohmann::json;

typedef map<string, string> Map;

std::string domain = getenv("FOREX_DOMAIN");
std::string access_token = getenv("FOREX_ACCESS_TOKEN");
std::string account_id = getenv("FOREX_ACCOUNT_ID");
std::string instruments = "EUR_USD,USD_CAD,EUR_JPY";

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

int main(int argc, char** argv) {
  const Context::Ptr context = new Context(Context::CLIENT_USE, "", "", "", Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");

  cout << domain << endl;
  cout << access_token << endl;
  cout << account_id << endl;

  // prepare session
  //URI uri(domain + "/v3/accounts");
  const Map& params = {
    //B bid A ask
    { "price", "BA" },
    { "from", "2016-10-17T15:17:00.000000000Z" },
    //M minute S second D day
    { "granularity", "M1" }
  };

  //URI uri(format("{0}/v3/accounts/{1}/pricing?instruments={2}", domain, account_id, instruments));

  cout << flatten_params(params) << endl;

  URI uri(format("{0}/v3/instruments/{1}/candles{2}", domain, "USD_JPY", flatten_params(params)));
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

  cout << res.getStatus() << endl;

  stringstream ss;
  StreamCopier::copyStream(is, ss);

  cout << ss.str() << endl;

  auto j = json::parse(ss.str());
  cout << j.dump(2) << endl;
}
