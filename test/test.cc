#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "../src/endpoints.hpp"
using namespace endpoints;

#include <json.hpp>
using nlohmann::json;

TEST_CASE("make sure endpoints work as expected") {
  const Map& params = {
    { "price", "BA" },
    { "from", "2017-10-01T15:17:00Z" },
    { "granularity", "D" }
  };

  REQUIRE(instrument::candles("USD_JPY", params) != nullptr);
}
