/*
Copyright (c) 2022 Snowplow Analytics Ltd. All rights reserved.

This program is licensed to you under the Apache License Version 2.0,
and you may not use this file except in compliance with the Apache License Version 2.0.
You may obtain a copy of the Apache License Version 2.0 at http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing,
software distributed under the Apache License Version 2.0 is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the Apache License Version 2.0 for the specific language governing permissions and limitations there under.
*/

#include "../../src/payload/self_describing_json.hpp"
#include "../catch.hpp"

using namespace snowplow;

TEST_CASE("self_describing_json") {
  json j = "{\"test\":\"event\"}"_json;
  SelfDescribingJson sdj("iglu:com.acme/test/jsonschema/1-0-0", j);

  REQUIRE("{\"data\":{\"test\":\"event\"},\"schema\":\"iglu:com.acme/test/jsonschema/1-0-0\"}" == sdj.to_string());
  json sdj_raw = sdj.get();
  REQUIRE("iglu:com.acme/test/jsonschema/1-0-0" == sdj_raw[SNOWPLOW_SCHEMA].get<std::string>());
  json sdj_data = sdj_raw[SNOWPLOW_DATA];
  REQUIRE("event" == sdj_data["test"].get<std::string>());
}
