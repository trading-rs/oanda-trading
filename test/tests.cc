#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "../src/endpoints.hpp"
using namespace endpoints;

#include <json.hpp>
using nlohmann::json;

#include <qtl_mysql.hpp>

TEST_CASE("make sure endpoints work as expected") {
  const Params& params = {
    { "price", "BA" },
    { "from", "2017-10-01T15:17:00Z" },
    { "granularity", "D" }
  };

  REQUIRE(instrument::candles("USD_JPY", params) != nullptr);
}

TEST_CASE("make sure mysql connection are all good") {
  const char* host = getenv("FOREX_DB_HOST");
  const char* account = getenv("FOREX_DB_ACCOUNT");
  const char* password = getenv("FOREX_DB_PASSWORD");
  const char* dbname = getenv("FOREX_DB_NAME");

  qtl::mysql::database db;
  auto conn_result = db.open(host, account, password, dbname);
  REQUIRE(conn_result);
}
