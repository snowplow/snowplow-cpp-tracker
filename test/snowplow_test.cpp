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

#include "../src/snowplow.hpp"
#include "catch.hpp"
#include "http/test_http_client.hpp"

using namespace snowplow;

using std::chrono::milliseconds;
using std::this_thread::sleep_for;

TEST_CASE("snowplow") {
  SECTION("return nullptr for unininitialized trackers") {
    REQUIRE(Snowplow::get_tracker("xyz") == nullptr);
  }

  SECTION("creates and retrieves a tracker") {
    auto tracker = Snowplow::create_tracker("ns", "https://collector.com", POST, "test-emitter.db");
    REQUIRE(tracker);
    REQUIRE(tracker->get_namespace() == "ns");

    auto got_tracker = Snowplow::get_tracker("ns");
    REQUIRE(got_tracker);
    REQUIRE(tracker.get() == got_tracker.get());

    REQUIRE(Snowplow::remove_tracker("ns"));
    REQUIRE(Snowplow::get_tracker("ns") == nullptr);
  }

  SECTION("setting default tracker") {
    // no default tracker on startup
    REQUIRE(!Snowplow::get_default_tracker());

    // sets the first tracker as default
    auto tracker1 = Snowplow::create_tracker("ns1", "https://collector.com", POST, "test-emitter.db");
    REQUIRE(Snowplow::get_default_tracker()->get_namespace() == "ns1");

    // doesn't change the default tracker when creating new ones
    auto tracker2 = Snowplow::create_tracker("ns2", "https://collector.com", POST, "test-emitter.db");
    REQUIRE(Snowplow::get_default_tracker()->get_namespace() == "ns1");

    // changes the default tracker on request
    Snowplow::set_default_tracker(tracker2);
    REQUIRE(Snowplow::get_default_tracker()->get_namespace() == "ns2");

    // register a new default tracker
    NetworkConfiguration network_config3("https://collector.com");
    EmitterConfiguration emitter_config3(make_shared<SqliteStorage>("test-emitter.db"));
    auto emitter3 = make_shared<Emitter>(network_config3, emitter_config3);
    auto tracker3 = make_shared<Tracker>(emitter3, nullptr, nullptr, "mob", "", "ns3");
    Snowplow::set_default_tracker(tracker3);
    REQUIRE(Snowplow::get_default_tracker()->get_namespace() == "ns3");
    REQUIRE(Snowplow::get_tracker("ns3"));

    REQUIRE(Snowplow::remove_tracker(tracker1));
    REQUIRE(Snowplow::remove_tracker(tracker2));
    REQUIRE(Snowplow::remove_tracker(tracker3));
  }

  SECTION("registers a custom tracker") {
    NetworkConfiguration network_config("https://collector.com");
    EmitterConfiguration emitter_config(make_shared<SqliteStorage>("test-emitter.db"));
    auto emitter = make_shared<Emitter>(network_config, emitter_config);
    auto tracker = make_shared<Tracker>(emitter, nullptr, nullptr, "mob", "", "custom");
    Snowplow::register_tracker(tracker);

    REQUIRE(Snowplow::get_tracker("custom")->get_namespace() == "custom");
    REQUIRE(Snowplow::get_default_tracker()->get_namespace() == "custom");

    REQUIRE(Snowplow::remove_tracker("custom"));
  }

  SECTION("handles multiple trackers") {
    auto storage1 = std::make_shared<SqliteStorage>("test-emitter-1.db");
    auto storage2 = std::make_shared<SqliteStorage>("test-emitter-2.db");

    auto http_client1 = new TestHttpClient();
    NetworkConfiguration network_config1("http://com.acme.collector", POST);
    network_config1.set_http_client(move(unique_ptr<TestHttpClient>(http_client1)));
    auto tracker1 = Snowplow::create_tracker(
      TrackerConfiguration("ns1"),
      network_config1,
      EmitterConfiguration(storage1)
    );

    auto http_client2 = new TestHttpClient();
    NetworkConfiguration network_config2("http://com.acme.collector", POST);
    network_config2.set_http_client(move(unique_ptr<TestHttpClient>(http_client2)));
    auto tracker2 = Snowplow::create_tracker(
      TrackerConfiguration("ns2"),
      network_config2,
      EmitterConfiguration(storage2)
    );

    tracker1->track(StructuredEvent("cat1", "act1"));
    tracker2->track(StructuredEvent("cat2", "act2"));
    tracker1->track(StructuredEvent("cat3", "act3"));

    tracker1->flush();
    tracker2->flush();

    REQUIRE(http_client1->get_instance_requests_list().size() == 2);
    REQUIRE(http_client2->get_instance_requests_list().size() == 1);

    TestHttpClient::reset();

    REQUIRE(Snowplow::remove_tracker("ns1"));
    REQUIRE(Snowplow::remove_tracker("ns2"));
  }

  SECTION("creates a common session and event store when given the same database path") {
    NetworkConfiguration network_config("http://com.acme.collector");
    SessionConfiguration session_config("test-emitter.db");
    EmitterConfiguration emitter_config("test-emitter.db");
    auto tracker = Snowplow::create_tracker(
        TrackerConfiguration("ns"), network_config, emitter_config, session_config);
    auto session_db = session_config.get_session_store();
    auto event_db = emitter_config.get_event_store();

    REQUIRE((SqliteStorage *)session_db.get() == (SqliteStorage *)event_db.get());
    REQUIRE(Snowplow::remove_tracker("ns"));
  }

  SECTION("creates different session and event stores given different paths") {
    NetworkConfiguration network_config("http://com.acme.collector");
    SessionConfiguration session_config("test-emitter-1.db");
    EmitterConfiguration emitter_config("test-emitter-2.db");
    auto tracker = Snowplow::create_tracker(
        TrackerConfiguration("ns"), network_config, emitter_config, session_config);
    auto session_db = session_config.get_session_store();
    auto event_db = emitter_config.get_event_store();

    REQUIRE((SqliteStorage *)session_db.get() != (SqliteStorage *)event_db.get());
    REQUIRE(Snowplow::remove_tracker("ns"));
  }
}
