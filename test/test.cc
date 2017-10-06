#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "../src/endpoints.hpp"
using namespace endpoints;

#include <json.hpp>
using nlohmann::json;

TEST_CASE("make sure endpoints work as expected") {
  const Map& params = {
    //B bid A ask
    { "price", "BA" },
    { "from", "2017-10-01T15:17:00Z" },
    //M1 1 minute, S1 1 second, D 1 day
    { "granularity", "D" }
  };

  REQUIRE(instrument::candles("USD_JPY", params) != nullptr);

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

  auto j = order::create(body);
  string id_s = j["lastTransactionID"];
  int id_i = stoi(id_s);

  REQUIRE(trade::all("EUR_USD") != nullptr);
  REQUIRE(trade::one(id_i) != nullptr);
  REQUIRE(trade::close(id_i) != nullptr);
}
