#include <json.hpp>
using nlohmann::json;

#include <string>
#include <iostream>
using namespace std;

#ifndef FORMAT_HEADER
#define FORMAT_HEADER
#include <fmt/format.h>
#include <fmt/format.cc>
#endif
using namespace fmt;

#include "src/endpoint.hpp"
using namespace endpoint;

int main(int argc, char** argv) {
  const Map& params = {
    //B bid A ask
    { "price", "BA" },
    { "from", "2016-10-17T15:17:00Z" },
    //M minute S second D day
    { "granularity", "M1" }
  };

  auto j = get(format("{0}/v3/instruments/{1}/candles", domain, "USD_JPY"), params);

  if (j != nullptr)
    cout << j.dump(2) << endl;

  json body = {
    { "order", {
        { "units", "100" },
        { "instrument", "EUR_USD" },
        { "timeInForce", "FOK" },
        { "type", "MARKET" },
        { "positionFill", "DEFAULT" }
      }
    }
  };

  auto j2 = post(format("{0}/v3/accounts/{1}/orders", domain, account_id), body);

  if (j2 != nullptr)
    cout << j.dump(2) << endl;
}
