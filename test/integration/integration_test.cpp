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

#include "../../include/snowplow/snowplow.hpp"
#include "micro.hpp"
#include "../catch.hpp"

using namespace snowplow;
using std::invalid_argument;
using std::runtime_error;
using std::to_string;
using std::this_thread::sleep_for;

TEST_CASE("integration") {
  SECTION("Tracks an event with desktop context") {
    Micro::clear();

    NetworkConfiguration network_config(SNOWPLOW_MICRO_ENDPOINT);
    EmitterConfiguration emitter_config("test-tracker.db");
    auto tracker = Snowplow::create_tracker(
        TrackerConfiguration("snowplow-testing", "snowplow-test-suite", pc),
        network_config,
        emitter_config);

    StructuredEvent sv("hello", "world");
    string event_id = tracker->track(sv);
    tracker->flush();

    auto counts = Micro::get_good_and_bad_count();
    REQUIRE(std::get<0>(counts) == 1);
    REQUIRE(std::get<1>(counts) == 0);

    auto good = Micro::get_good();
    REQUIRE(good.size() == 1);
    auto first = good.front();
    auto event = first["event"].get<json>();
    REQUIRE(event["platform"] == "pc");
    REQUIRE(event["event_id"] == event_id);
    REQUIRE(event["se_category"] == "hello");
    REQUIRE(event["se_action"] == "world");

    auto entities = event["contexts"].get<json>()["data"].get<list<json>>();
    REQUIRE(entities.size() == 1);
    REQUIRE(entities.front()["schema"] == SNOWPLOW_SCHEMA_DESKTOP_CONTEXT);
    auto entity = entities.front()["data"].get<json>();
    REQUIRE(entity["osType"] == Utils::get_os_type());

    Snowplow::remove_tracker(tracker);
  }

  SECTION("Tracks events with session") {
    Micro::clear();

    NetworkConfiguration network_config(SNOWPLOW_MICRO_ENDPOINT);
    EmitterConfiguration emitter_config("test-tracker.db");
    TrackerConfiguration tracker_config("snowplow-testing", "snowplow-test-suite", pc);
    tracker_config.set_desktop_context(false);
    SessionConfiguration session_config("test-tracker.db");
    auto tracker = Snowplow::create_tracker(tracker_config, network_config, emitter_config, session_config);

    string event_id = tracker->track(StructuredEvent("hello", "world1"));
    tracker->track(StructuredEvent("hello", "world2"));
    tracker->flush();

    auto counts = Micro::get_good_and_bad_count();
    REQUIRE(std::get<0>(counts) == 2);
    REQUIRE(std::get<1>(counts) == 0);

    auto good = Micro::get_good();
    REQUIRE(good.size() == 2);

    // check first event session context
    auto event = good.front()["event"].get<json>();
    auto entities = event["contexts"].get<json>()["data"].get<list<json>>();
    REQUIRE(entities.size() == 1);
    REQUIRE(entities.front()["schema"] == SNOWPLOW_SCHEMA_CLIENT_SESSION);
    auto entity1 = entities.front()["data"].get<json>();
    REQUIRE(entity1["firstEventId"] == event_id);

    // second event has the same session information
    event = good.back()["event"].get<json>();
    entities = event["contexts"].get<json>()["data"].get<list<json>>();
    auto entity2 = entities.front()["data"].get<json>();
    REQUIRE(entity2["firstEventId"] == event_id);
    REQUIRE(entity2["userId"] == entity1["userId"]);
    REQUIRE(entity2["sessionId"] == entity1["sessionId"]);

    Snowplow::remove_tracker(tracker);
  }

  SECTION("Tracks an event with custom context") {
    Micro::clear();

    NetworkConfiguration network_config(SNOWPLOW_MICRO_ENDPOINT);
    EmitterConfiguration emitter_config("test-tracker.db");
    TrackerConfiguration tracker_config("snowplow-testing", "snowplow-test-suite", pc);
    tracker_config.set_desktop_context(false);
    auto tracker = Snowplow::create_tracker(tracker_config, network_config, emitter_config);

    const string link_click_schema = "iglu:com.snowplowanalytics.snowplow/link_click/jsonschema/1-0-1";
    const string media_player_schema = "iglu:com.snowplowanalytics.snowplow/media_player/jsonschema/1-0-0";
    SelfDescribingEvent sde(SelfDescribingJson(
      link_click_schema,
      "{\"targetUrl\": \"http://a-target-url.com\"}"_json
    ));
    sde.set_context({SelfDescribingJson(
        media_player_schema,
        "{\"currentTime\": 0, \"duration\": 10, \"ended\": false, \"loop\": false, \"muted\": false, \"paused\": false, \"playbackRate\": 1, \"volume\": 100}"_json)});
    string event_id = tracker->track(sde);
    tracker->flush();

    auto counts = Micro::get_good_and_bad_count();
    REQUIRE(std::get<0>(counts) == 1);
    REQUIRE(std::get<1>(counts) == 0);

    auto good = Micro::get_good();
    REQUIRE(good.size() == 1);
    auto first = good.front();
    auto event = first["event"].get<json>();
    REQUIRE(event["unstruct_event"].get<json>()["data"].get<json>()["schema"] == link_click_schema);
    auto entities = event["contexts"].get<json>()["data"].get<list<json>>();

    REQUIRE(entities.size() == 1);
    REQUIRE(entities.front()["schema"] == media_player_schema);
    auto entity = entities.front()["data"].get<json>();
    REQUIRE(entity["duration"] == 10);

    Snowplow::remove_tracker(tracker);
  }

  SECTION("Tracks an event without flushing") {
    Micro::clear();

    auto tracker = Snowplow::create_tracker("snowplow-testing", SNOWPLOW_MICRO_ENDPOINT, POST, "test-tracker.db");

    ScreenViewEvent event;
    string name = "screen";
    event.name = &name;
    tracker->track(event);
    sleep_for(milliseconds(500));

    auto counts = Micro::get_good_and_bad_count();
    REQUIRE(std::get<0>(counts) == 1);
    REQUIRE(std::get<1>(counts) == 0);

    Snowplow::remove_tracker(tracker);
  }

}
