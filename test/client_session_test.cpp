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

#include "catch.hpp"
#include "../src/client_session.hpp"

TEST_CASE("client_session") {
  SECTION("The Session must persist and update in the background") {
    Storage::init("test1.db")->delete_all_session_rows();
    
    ClientSession cs("test1.db", 500, 500, 250);
    cs.start();

    // Can only start once
    cs.start();
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

    this_thread::sleep_for(chrono::milliseconds(875));
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

  SECTION("The Session must fetch information from previous sessions") {
    Storage::init("test2.db")->delete_all_session_rows();

    ClientSession cs("test2.db", 10000, 10000, 5000);
    cs.start();
    SelfDescribingJson session_json = cs.get_session_context("event-id2");
    cs.stop();

    json data = session_json.get()[SNOWPLOW_DATA];
    REQUIRE(1 == data[SNOWPLOW_SESSION_INDEX].get<unsigned long long>());

    ClientSession cs1("test2.db", 500, 500, 250);
    cs1.start();

    SelfDescribingJson session_json1 = cs1.get_session_context("event-id2");
    REQUIRE("iglu:com.snowplowanalytics.snowplow/client_session/jsonschema/1-0-1" == session_json1.get()[SNOWPLOW_SCHEMA].get<std::string>());
    
    json data1 = session_json1.get()[SNOWPLOW_DATA];

    REQUIRE("event-id2" == data1[SNOWPLOW_SESSION_FIRST_ID].get<std::string>());
    REQUIRE("SQLITE" == data1[SNOWPLOW_SESSION_STORAGE].get<std::string>());
    REQUIRE(2 == data1[SNOWPLOW_SESSION_INDEX].get<unsigned long long>());

    string user_id1 = data1[SNOWPLOW_SESSION_USER_ID].get<std::string>();
    string current_id1 = data1[SNOWPLOW_SESSION_ID].get<std::string>();

    this_thread::sleep_for(chrono::milliseconds(850));
    session_json1 = cs1.get_session_context("event-id2");
    data1 = session_json1.get()[SNOWPLOW_DATA];

    REQUIRE("event-id2" == data1[SNOWPLOW_SESSION_FIRST_ID].get<std::string>());
    REQUIRE("SQLITE" == data1[SNOWPLOW_SESSION_STORAGE].get<std::string>());
    REQUIRE(3 == data1[SNOWPLOW_SESSION_INDEX].get<unsigned long long>());
    REQUIRE(user_id1 == data1[SNOWPLOW_SESSION_USER_ID].get<std::string>());
    REQUIRE(current_id1 != data1[SNOWPLOW_SESSION_ID].get<std::string>());
    REQUIRE(current_id1 == data1[SNOWPLOW_SESSION_PREVIOUS_ID].get<std::string>());

    cs1.stop();
  }

  SECTION("If corrupted data makes it into the session database entry use defaults") {
    Storage::init("test3.db")->insert_update_session("{}"_json);

    ClientSession cs("test3.db", 500, 500, 250);
    cs.start();

    SelfDescribingJson session_json = cs.get_session_context("event-id3");
    REQUIRE("iglu:com.snowplowanalytics.snowplow/client_session/jsonschema/1-0-1" == session_json.get()[SNOWPLOW_SCHEMA].get<std::string>());
    
    json data = session_json.get()[SNOWPLOW_DATA];

    REQUIRE("event-id3" == data[SNOWPLOW_SESSION_FIRST_ID].get<std::string>());
    REQUIRE("SQLITE" == data[SNOWPLOW_SESSION_STORAGE].get<std::string>());
    REQUIRE(1 == data[SNOWPLOW_SESSION_INDEX].get<unsigned long long>());

    string user_id = data[SNOWPLOW_SESSION_USER_ID].get<std::string>();
    string current_id = data[SNOWPLOW_SESSION_ID].get<std::string>();

    this_thread::sleep_for(chrono::milliseconds(850));
    session_json = cs.get_session_context("event-id3");
    data = session_json.get()[SNOWPLOW_DATA];

    REQUIRE("event-id3" == data[SNOWPLOW_SESSION_FIRST_ID].get<std::string>());
    REQUIRE("SQLITE" == data[SNOWPLOW_SESSION_STORAGE].get<std::string>());
    REQUIRE(2 == data[SNOWPLOW_SESSION_INDEX].get<unsigned long long>());
    REQUIRE(user_id == data[SNOWPLOW_SESSION_USER_ID].get<std::string>());
    REQUIRE(current_id != data[SNOWPLOW_SESSION_ID].get<std::string>());
    REQUIRE(current_id == data[SNOWPLOW_SESSION_PREVIOUS_ID].get<std::string>());

    cs.stop();
  }
}
