#include <json.hpp>
#include <string>
#include <utility>
#include <iostream>
#include <memory>

#ifndef FORMAT_HEADER
#define FORMAT_HEADER
#include <fmt/format.h>
#include <fmt/format.cc>
#endif

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
using nlohmann::json;

std::string domain = getenv("FOREX_DOMAIN");
std::string access_token = getenv("FOREX_ACCESS_TOKEN");
std::string account_id = getenv("FOREX_ACCOUNT_ID");
std::string instruments = "EUR_USD,USD_CAD,EUR_JPY";

int main(int argc, char** argv) {
  const Context::Ptr context = new Context(Context::CLIENT_USE, "", "", "", Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");

  cout << domain << endl;
  cout << access_token << endl;
  cout << account_id << endl;

  // prepare session
  //URI uri(domain + "/v3/accounts");
  URI uri(format("{0}/v3/accounts/{1}/pricing?instruments={2}", domain, account_id, instruments));
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
  istream& is = session.receiveResponse(res);

  cout << res.getStatus() << endl;

  stringstream ss;
  StreamCopier::copyStream(is, ss);

  cout << ss.str() << endl;

  auto j = json::parse(ss.str());
  cout << j.dump(2) << endl;
}
