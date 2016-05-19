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

#include "../include/catch.hpp"
#include "../src/client_session.hpp"

TEST_CASE("client_session") {
  Storage::instance("test.db")->delete_all_session_rows();

  ClientSession cs("test.db", 100, 100, 50);
  cs.start();

  REQUIRE(false == cs.get_is_background());
  cs.set_is_background(true);
  REQUIRE(true == cs.get_is_background());

  SelfDescribingJson session_json = cs.get_session_context("event-id");
  REQUIRE("iglu:com.snowplowanalytics.snowplow/client_session/jsonschema/1-0-1" == session_json.get()[SNOWPLOW_SCHEMA].get<std::string>());
  
  json data = session_json.get()[SNOWPLOW_DATA];

  REQUIRE("event-id" == data[SNOWPLOW_SESSION_FIRST_ID].get<std::string>());
  REQUIRE("SQLITE" == data[SNOWPLOW_SESSION_STORAGE].get<std::string>());
  REQUIRE(1 == data[SNOWPLOW_SESSION_INDEX].get<unsigned long long>());

  string user_id = data[SNOWPLOW_SESSION_USER_ID].get<std::string>();
  string current_id = data[SNOWPLOW_SESSION_ID].get<std::string>();

  this_thread::sleep_for(chrono::milliseconds(125));
  session_json = cs.get_session_context("event-id");
  data = session_json.get()[SNOWPLOW_DATA];

  REQUIRE("event-id" == data[SNOWPLOW_SESSION_FIRST_ID].get<std::string>());
  REQUIRE("SQLITE" == data[SNOWPLOW_SESSION_STORAGE].get<std::string>());
  REQUIRE(2 == data[SNOWPLOW_SESSION_INDEX].get<unsigned long long>());
  REQUIRE(user_id == data[SNOWPLOW_SESSION_USER_ID].get<std::string>());
  REQUIRE(current_id != data[SNOWPLOW_SESSION_ID].get<std::string>());
  REQUIRE(current_id == data[SNOWPLOW_SESSION_PREVIOUS_ID].get<std::string>());

  cs.stop();
}
