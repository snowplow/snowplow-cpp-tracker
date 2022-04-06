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

#include "../../include/json.hpp"
#include "../../src/payload/payload.hpp"
#include "../../src/utils.hpp"
#include "../catch.hpp"
#include <map>
#include <string>

using namespace snowplow;
using json = nlohmann::json;

TEST_CASE("payload") {
  Payload pl;
  REQUIRE(pl.get().size() == 0);

  SECTION("add should only push a kv pair if both key and value are not empty") {
    pl.add("", "world");
    REQUIRE(pl.get().size() == 0);
    pl.add("hello", "");
    REQUIRE(pl.get().size() == 0);
    pl.add("hello", "world");
    REQUIRE(pl.get().size() == 1);
    REQUIRE(pl.get()["hello"] == "world");
  }

  SECTION("add_map should add all valid non-empty kv pairs") {
    map<string, string> test_map = {{"hello", "world"}, {"e", "pv"}};
    pl.add_map(test_map);
    REQUIRE(pl.get().size() == 2);
    REQUIRE(pl.get()["hello"] == "world");
    REQUIRE(pl.get()["e"] == "pv");
  }

  SECTION("add_payload should add all the entries in one payload to this one") {
    Payload pl2;
    pl2.add("hello", "world");
    pl.add_payload(pl2);
    REQUIRE(pl.get().size() == 1);
    REQUIRE(pl.get()["hello"] == "world");
  }

  SECTION("add_json should correctly store a JSON as a string") {
    json j1 = "{ \"happy\": true, \"pi\": 3.141 }"_json;
    json j2 = "{ \"happy\": true, \"pi\": 3.141 }"_json;
    pl.add_json(j1, true, "cx", "co");
    pl.add_json(j2, false, "cx", "co");
    REQUIRE(pl.get().size() == 2);
    REQUIRE(pl.get()["cx"] == "eyJoYXBweSI6dHJ1ZSwicGkiOjMuMTQxfQ==");
    REQUIRE(pl.get()["co"] == "{\"happy\":true,\"pi\":3.141}");
  }
}
