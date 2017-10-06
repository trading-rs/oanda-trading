#include <json.hpp>
using nlohmann::json;

#include <string>
#include <iostream>
using namespace std;

#include "src/endpoints.hpp"
using namespace endpoints;

auto print_result(const json &result) -> void {
  if (result != nullptr)
    cout << result.dump() << endl;
}

int main(int argc, char** argv) {
  const Map& params = {
    //B bid A ask
    { "price", "BA" },
    { "from", "2017-10-01T15:17:00Z" },
    //M1 1 minute, S1 1 second, D 1 day
    { "granularity", "D" }
  };

  print_result(instrument::candles("USD_JPY", params));

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
  string id = j["lastTransactionID"];

  print_result(trade::all("EUR_USD"));
  print_result(trade::one(11));
  print_result(trade::close(std::stoi(id)));
}
