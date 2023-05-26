/*
Copyright (c) 2023 Snowplow Analytics Ltd. All rights reserved.

This program is licensed to you under the Apache License Version 2.0,
and you may not use this file except in compliance with the Apache License Version 2.0.
You may obtain a copy of the Apache License Version 2.0 at http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing,
software distributed under the Apache License Version 2.0 is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the Apache License Version 2.0 for the specific language governing permissions and limitations there under.
*/

#include "../../include/snowplow/configuration/network_configuration.hpp"
#include "../catch.hpp"

using namespace snowplow;

TEST_CASE("network configuration") {
  SECTION("throws an error for empty collector URL") {
    bool error = false;

    try { NetworkConfiguration(""); }
    catch (std::invalid_argument) { error = true; }

    REQUIRE(error);
  }

  SECTION("parses URL with http://") {
    NetworkConfiguration config("http://com.acme.collector", POST);

    REQUIRE(HTTP == config.get_protocol());
    REQUIRE("com.acme.collector" == config.get_collector_hostname());
    REQUIRE(POST == config.get_method());
  }

  SECTION("parses URL with capital HTTP://") {
    NetworkConfiguration config("HTTP://com.acme.collector", POST);

    REQUIRE(HTTP == config.get_protocol());
    REQUIRE("com.acme.collector" == config.get_collector_hostname());
    REQUIRE(POST == config.get_method());
  }

  SECTION("parses URL with https://") {
    NetworkConfiguration config("https://collect.snplw.com", GET);

    REQUIRE(HTTPS == config.get_protocol());
    REQUIRE("collect.snplw.com" == config.get_collector_hostname());
    REQUIRE(GET == config.get_method());
  }

  SECTION("parses URL without protocol") {
    NetworkConfiguration config("com.acme.collector", GET);

    REQUIRE(HTTPS == config.get_protocol());
    REQUIRE("com.acme.collector" == config.get_collector_hostname());
    REQUIRE(GET == config.get_method());
  }
}
