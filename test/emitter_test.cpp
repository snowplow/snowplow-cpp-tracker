/*
Copyright (c) 2016 Snowplow Analytics Ltd. All rights reserved.

This program is licensed to you under the Apache License Version 2.0,
and you may not use this file except in compliance with the Apache License Version 2.0.
You may obtain a copy of the Apache License Version 2.0 at http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing,
software distributed under the Apache License Version 2.0 is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the Apache License Version 2.0 for the specific language governing permissions and limitations there under.
*/

#include "catch.hpp"
#include "../src/emitter.hpp"

TEST_CASE("emitter") {
  SECTION("Emitter setup confirmation") {
    Emitter emitter("com.acme.collector", Emitter::Method::POST, Emitter::Protocol::HTTP, 500, 52000, 51000, "test.db");

    REQUIRE(false == emitter.is_running());
    REQUIRE("http://com.acme.collector/com.snowplowanalytics.snowplow/tp2" == emitter.get_cracked_url().to_string());
    REQUIRE(Emitter::Method::POST == emitter.get_method());
    REQUIRE(500 == emitter.get_send_limit());
    REQUIRE(52000 == emitter.get_byte_limit_post());
    REQUIRE(51000 == emitter.get_byte_limit_get());

    emitter.start();
    REQUIRE(true == emitter.is_running());
    emitter.flush();
    REQUIRE(false == emitter.is_running());
    emitter.start();
    REQUIRE(true == emitter.is_running());
    emitter.stop();
    REQUIRE(false == emitter.is_running());

    Emitter emitter_1("com.acme.collector", Emitter::Method::GET, Emitter::Protocol::HTTPS, 500, 52000, 51000, "test.db");

    REQUIRE(false == emitter_1.is_running());
    REQUIRE("https://com.acme.collector/i" == emitter_1.get_cracked_url().to_string());
    REQUIRE(Emitter::Method::GET == emitter_1.get_method());
    REQUIRE(500 == emitter_1.get_send_limit());
    REQUIRE(52000 == emitter_1.get_byte_limit_post());
    REQUIRE(51000 == emitter_1.get_byte_limit_get());
  }

  SECTION("Emitter should remove successful events from the database") {
    Emitter emitter("com.acme.collector", Emitter::Method::GET, Emitter::Protocol::HTTPS, 500, 52000, 51000, "test.db");
  }
}
