#pragma once

#include <json.hpp>
using nlohmann::json;

#include <string>
#include <iostream>
using namespace std;

#include <range/v3/all.hpp>
using namespace ranges;

#ifndef FORMAT_HEADER
#define FORMAT_HEADER
#include <fmt/format.h>
#include <fmt/format.cc>
#endif
using namespace fmt;

#include "api.hpp"
using namespace api;

namespace endpoints {
  namespace instrument {
    auto candles(string instrument, const Params &params) -> json {
      return get(format("instruments/{}/candles", instrument), params);
    }

    auto order_book(string instrument, const Params &params) -> json {
      return get(format("instruments/{}/orderBook", instrument), params);
    }

    auto position_book(string instrument, const Params &params) -> json {
      return get(format("instruments/{}/positionBook", instrument), params);
    }
  }

  namespace trade {
    auto all(const string &instrument) -> json {
      return get(format("accounts/{}/trades", account_id), Params({{ "instrument", instrument }}));
    }

    auto one(int order_id) -> json {
      return get(format("accounts/{0}/trades/{1}", account_id, order_id));
    }

    auto close(int order_id) -> json {
      return put(format("accounts/{0}/trades/{1}/close", account_id, order_id));
    }
  }

  namespace order {
    auto all(const string &instrument) -> json {
      return get(format("accounts/{}/orders", account_id), Params({{ "instrument", instrument }}));
    }

    auto one(int order_id) -> json {
      return get(format("accounts/{0}/orders/{1}", account_id, order_id));
    }

    auto create(const json &body) -> json {
      return post(format("accounts/{}/orders", account_id), body);
    }

    auto cancel(int order_id) -> json {
      return put(format("accounts/{0}/orders/{1}/cancel", account_id, order_id));
    }
  }

  namespace pricing {
    // single instrument { "EUR_USD" }
    // multiple instruments { "EUR_USD", "USD_CAD" }
    auto pricing(vector<string> instruments) -> json {
      string instruments_str = instruments | view::join(',');
      const Params &params = {
        { "instruments", instruments_str }
      };
      return get(format("accounts/{}/pricing", domain, account_id), params);
    }
  }
}
